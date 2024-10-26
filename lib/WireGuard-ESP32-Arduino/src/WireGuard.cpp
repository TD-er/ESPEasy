/*
 * WireGuard implementation for ESP32 Arduino by Kenta Ida (fuga@fugafuga.org)
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "WireGuard-ESP32.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_netif.h"
#include "esp_system.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/ip.h"
#include "lwip/netdb.h"

#include "esp32-hal-log.h"

extern "C" {
#include "wireguardif.h"
#include "wireguard-platform.h"
}

#define TAG "[WireGuard] "

struct netif_add_and_up_parameters {
	const ip4_addr_t *ipaddr;
	const ip4_addr_t *netmask;
	const ip4_addr_t *gw;
	struct netif **wg_netif;
	struct netif *wg_netif_struct;
	void *state;
};

static esp_err_t netif_add_and_up_in_lwip_ctx(void *ctx) {
	netif_add_and_up_parameters *param = static_cast<netif_add_and_up_parameters *>(ctx);

	// Register the new WireGuard network interface with lwIP
	*param->wg_netif = netif_add(param->wg_netif_struct, param->ipaddr, param->netmask, param->gw, param->state, &wireguardif_init, &ip_input);
	if (!param->wg_netif) {
		return ESP_FAIL;
	}

	// Mark the interface as administratively up, link up flag is set automatically when peer connects
	netif_set_up(*param->wg_netif);

	return ESP_OK;
}

static esp_err_t netif_set_default_in_lwip_ctx(void *ctx) {
	netif *nif = static_cast<netif *>(ctx);
	netif_set_default(nif);
	return ESP_OK;
}

bool WireGuard::begin(const IPAddress& localIP,
                      const IPAddress& Subnet,
                      const uint16_t localPort,
                      const IPAddress& Gateway,
                      const char* privateKey,
                      const char* remotePeerAddress,
                      const char* remotePeerPublicKey,
                      uint16_t remotePeerPort,
                      const IPAddress &allowedIP,
                      const IPAddress &allowedMask,
                      bool make_default,
                      const char *preshared_key,
					  int (*in_filter_fn)(struct pbuf*),
					  int (*out_filter_fn)(struct pbuf*)) {
	struct wireguardif_init_data wg;
	struct wireguardif_peer peer;
	ip_addr_t ipaddr = IPADDR4_INIT(static_cast<uint32_t>(localIP));
	ip_addr_t netmask = IPADDR4_INIT(static_cast<uint32_t>(Subnet));
	ip_addr_t gateway = IPADDR4_INIT(static_cast<uint32_t>(Gateway));
	ip_addr_t allowed_ip = IPADDR4_INIT(static_cast<uint32_t>(allowedIP));
	ip_addr_t allowed_mask = IPADDR4_INIT(static_cast<uint32_t>(allowedMask));

	assert(privateKey != NULL);
	assert(remotePeerAddress != NULL);
	assert(remotePeerPublicKey != NULL);
	assert(remotePeerPort != 0);

	// Setup the WireGuard device structure
	wg.private_key = privateKey;
	wg.listen_port = localPort;

	wg.bind_netif = NULL;
	wg.in_filter_fn = in_filter_fn;
	wg.out_filter_fn = out_filter_fn;

	// Initialise the first WireGuard peer structure
	wireguardif_peer_init(&peer);
	// If we know the endpoint's address can add here
	bool success_get_endpoint_ip = false;
	for(int retry = 0; retry < 5; retry++) {
		ip_addr_t endpoint_ip = IPADDR4_INIT_BYTES(0, 0, 0, 0);
		struct addrinfo *res = NULL;
		struct addrinfo hint;
		memset(&hint, 0, sizeof(hint));
		memset(&endpoint_ip, 0, sizeof(endpoint_ip));
		if( lwip_getaddrinfo(remotePeerAddress, NULL, &hint, &res) != 0 ) {
			vTaskDelay(pdMS_TO_TICKS(2000));
			continue;
		}
		success_get_endpoint_ip = true;
		struct in_addr addr4 = ((struct sockaddr_in *) (res->ai_addr))->sin_addr;
		inet_addr_to_ip4addr(ip_2_ip4(&endpoint_ip), &addr4);
		lwip_freeaddrinfo(res);

		peer.endpoint_ip = endpoint_ip;
		log_i(TAG "%s is %3d.%3d.%3d.%3d"
			, remotePeerAddress
			, (endpoint_ip.u_addr.ip4.addr >>  0) & 0xff
			, (endpoint_ip.u_addr.ip4.addr >>  8) & 0xff
			, (endpoint_ip.u_addr.ip4.addr >> 16) & 0xff
			, (endpoint_ip.u_addr.ip4.addr >> 24) & 0xff
			);
		break;
	}
	if( !success_get_endpoint_ip  ) {
		log_e(TAG "failed to get endpoint ip.");
		return false;
	}

	netif_add_and_up_parameters params = {
		ip_2_ip4(&ipaddr),
		ip_2_ip4(&netmask),
		ip_2_ip4(&gateway),
		&wg_netif,
		&wg_netif_struct,
		&wg,
	};
	esp_netif_tcpip_exec(netif_add_and_up_in_lwip_ctx, &params);
	if( wg_netif == nullptr ) {
		log_e(TAG "failed to initialize WG netif.");
		return false;
	}

	peer.public_key = remotePeerPublicKey;
	peer.preshared_key = preshared_key;

	peer.allowed_ip = allowed_ip;
	peer.allowed_mask = allowed_mask;

	peer.endport_port = remotePeerPort;

	// Initialize the platform
	wireguard_platform_init();
	// Register the new WireGuard peer with the netwok interface
	wireguardif_add_peer(wg_netif, &peer, &wireguard_peer_index);
	if ((wireguard_peer_index != WIREGUARDIF_INVALID_INDEX) && !ip_addr_isany(&peer.endpoint_ip)) {
		// Start outbound connection to peer
		log_i(TAG "connecting wireguard...");
		wireguardif_connect(wg_netif, wireguard_peer_index);
		// Save the current default interface for restoring when shutting down the WG interface.
		previous_default_netif = netif_default;
		// Set default interface to WG device.
		if (make_default)
			esp_netif_tcpip_exec(netif_set_default_in_lwip_ctx, wg_netif);
	}

	this->_is_initialized = true;
	return true;
}

static esp_err_t shutdown_and_remove_in_lwip_ctx(void *ctx) {
	netif *nif = static_cast<netif *>(ctx);

	// Shutdown the wireguard interface.
	wireguardif_shutdown(nif);
	// Remove the WG interface;
	netif_remove(nif);

	return ESP_OK;
}

void WireGuard::end() {
	if( !this->_is_initialized ) return;

	// Restore the default interface.
	esp_netif_tcpip_exec(netif_set_default_in_lwip_ctx, previous_default_netif);
	previous_default_netif = nullptr;
	// Disconnect the WG interface.
	wireguardif_disconnect(wg_netif, wireguard_peer_index);
	// Remove peer from the WG interface
	wireguardif_remove_peer(wg_netif, wireguard_peer_index);
	wireguard_peer_index = WIREGUARDIF_INVALID_INDEX;

	esp_netif_tcpip_exec(shutdown_and_remove_in_lwip_ctx, wg_netif);
	wg_netif = nullptr;

	this->_is_initialized = false;
}

bool WireGuard::is_peer_up(ip_addr_t *current_ip, uint16_t *current_port) {
	if (!this->_is_initialized) return false;

	return wireguardif_peer_is_up(wg_netif, wireguard_peer_index, current_ip, current_port) == ERR_OK;
};

WireGuard::WireGuard() {
	bzero(&wg_netif_struct, sizeof(wg_netif_struct));
}
