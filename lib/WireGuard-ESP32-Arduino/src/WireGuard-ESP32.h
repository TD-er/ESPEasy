/*
 * WireGuard implementation for ESP32 Arduino by Kenta Ida (fuga@fugafuga.org)
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once
#include <IPAddress.h>

#include "lwip/ip_addr.h"
#include "lwip/pbuf.h"

extern "C" {
 #include "wireguardif.h"
}

class WireGuard
{
private:
	bool _is_initialized = false;
	struct netif wg_netif_struct;
	struct netif *wg_netif = NULL;
	struct netif *previous_default_netif = NULL;
	uint8_t wireguard_peer_index = WIREGUARDIF_INVALID_INDEX;

public:
	WireGuard();

	bool begin(// Private address that this device will have in the WireGuard network
	           const IPAddress& localIP,
	           // Subnet of the WireGuard network
	           const IPAddress& Subnet,
			   // Port that the interface will try to bind to
			   const uint16_t localPort,
	           // Gateway of the WireGuard network
	           const IPAddress& Gateway,
	           // Our private key
	           const char* privateKey,
	           // Public address of the peer to connect to
	           const char* remotePeerAddress,
	           // The peers public key
	           const char* remotePeerPublicKey,
	           // Port of the peer
	           uint16_t remotePeerPort,
	           // IP allowed as source in received packets
	           const IPAddress &allowedIP = IPAddress(0, 0, 0, 0),
	           // Subnet allowed as source in received packets
	           const IPAddress &allowedMask = IPAddress(255, 255, 255, 255),
	           // Make WireGuard the default interface for non-local traffic
	           // (i.e. traffic requiring passing through an interface's gateway)
	           bool make_default = true,
	           // Optional preshared key for this connection.
	           const char *preshared_key = nullptr,
			   int (*in_filter_fn)(struct pbuf*) = nullptr,
			   int (*out_filter_fn)(struct pbuf*) = nullptr);

	void end();
	bool is_initialized() const { return this->_is_initialized; }

	bool is_peer_up(ip_addr_t *current_ip, uint16_t *current_port);
};
