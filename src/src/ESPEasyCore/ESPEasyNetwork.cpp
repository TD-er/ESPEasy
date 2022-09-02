#include "../ESPEasyCore/ESPEasyNetwork.h"

#include "../ESPEasyCore/ESPEasy_Log.h"
#include "../ESPEasyCore/ESPEasyEth.h"
#include "../ESPEasyCore/ESPEasyWifi.h"

#ifdef USES_ESPEASY_NOW
#include "../Globals/ESPEasy_now_state.h"
#include "../Globals/ESPEasy_now_handler.h"
#endif

#include "../Globals/ESPEasyWiFiEvent.h"
#include "../Globals/NetworkState.h"
#include "../Globals/Settings.h"

#include "../Helpers/Network.h"
#include "../Helpers/StringConverter.h"
#include "../Helpers/MDNS_Helper.h"

#if FEATURE_ETHERNET
#include <ETH.h>
#endif

void setNetworkMedium(NetworkMedium_t new_medium) {
  if (active_network_medium == new_medium) {
    return;
  }
  bool process_exit_active_medium = true;
  if (new_medium == NetworkMedium_t::ESPEasyNOW_only) {
    if (!Settings.UseESPEasyNow()) {
      return;
    }
    if (WiFiEventData.unprocessedWifiEvents() ||
        WiFiEventData.wifiConnectInProgress) { 
      return; 
    }
    if (WiFiEventData.WiFiServicesInitialized()) {
      return;
    }
    if (active_network_medium != NetworkMedium_t::WIFI) {
      // Only allow to set to ESPEasyNOW_only from WiFi
      return;
    }
    #ifdef USES_ESPEASY_NOW
    if (use_EspEasy_now) {
      ESPEasy_now_handler.end();
      active_network_medium = new_medium;
      ESPEasy_now_handler.begin();
      process_exit_active_medium = false;
    }

    #endif
  }
  if (process_exit_active_medium) {
    switch (active_network_medium) {
      case NetworkMedium_t::Ethernet:
        #if FEATURE_ETHERNET
        // FIXME TD-er: How to 'end' ETH?
  //      ETH.end();
        if (new_medium == NetworkMedium_t::WIFI) {
          WiFiEventData.clearAll();
        }
        #endif
        break;
      case NetworkMedium_t::WIFI:
        WiFiEventData.timerAPoff.setMillisFromNow(WIFI_AP_OFF_TIMER_DURATION);
        WiFiEventData.timerAPstart.clear();
        if (new_medium == NetworkMedium_t::Ethernet) {
          WifiDisconnect();
        }
        break;
      case NetworkMedium_t::ESPEasyNOW_only:
        #ifdef USES_ESPEASY_NOW
        if (use_EspEasy_now) {
          ESPEasy_now_handler.end();
        }
        #endif

        //WiFiEventData.clearAll();
        break;
      case NetworkMedium_t::NotSet:
        break;
    }
  }
  statusLED(true);
  active_network_medium = new_medium;
  last_network_medium_set_moment.setNow();
  addLog(LOG_LEVEL_INFO, String(F("Set Network mode: ")) + toString(active_network_medium));
}

bool isESPEasy_now_only() {
  #ifdef USES_ESPEASY_NOW
    if (active_network_medium == NetworkMedium_t::ESPEasyNOW_only) {
      return true;
    }
    if (use_EspEasy_now) {
      return !NetworkConnected();
    }
  #endif
  return false;
}


/*********************************************************************************************\
   Ethernet or Wifi Support for ESP32 Build flag FEATURE_ETHERNET
\*********************************************************************************************/
void NetworkConnectRelaxed() {
  if (NetworkConnected()) return;
#if FEATURE_ETHERNET
  if(active_network_medium == NetworkMedium_t::Ethernet) {
    if (ETHConnectRelaxed()) {
      return;
    }
    // Failed to start the Ethernet network, probably not present of wrong parameters.
    // So set the runtime active medium to WiFi to try connecting to WiFi or at least start the AP.
    setNetworkMedium(NetworkMedium_t::WIFI);
  }
#endif
  WiFiConnectRelaxed();
}

bool NetworkConnected() {
  #if FEATURE_ETHERNET
  if(active_network_medium == NetworkMedium_t::Ethernet) {
    return ETHConnected();
  }
  #endif
  return WiFiConnected();
}

IPAddress NetworkLocalIP() {
  #if FEATURE_ETHERNET
  if(active_network_medium == NetworkMedium_t::Ethernet) {
    if(EthEventData.ethInitSuccess) {
      return ETH.localIP();
    } else {
      addLog(LOG_LEVEL_ERROR, F("Call NetworkLocalIP() only on connected Ethernet!"));
      return IPAddress();
    }
  }
  #endif
  return WiFi.localIP();
}

IPAddress NetworkSubnetMask() {
  #if FEATURE_ETHERNET
  if(active_network_medium == NetworkMedium_t::Ethernet) {
    if(EthEventData.ethInitSuccess) {
      return ETH.subnetMask();
    } else {
      addLog(LOG_LEVEL_ERROR, F("Call NetworkSubnetMask() only on connected Ethernet!"));
      return IPAddress();
    }
  }
  #endif
  return WiFi.subnetMask();
}

IPAddress NetworkGatewayIP() {
  #if FEATURE_ETHERNET
  if(active_network_medium == NetworkMedium_t::Ethernet) {
    if(EthEventData.ethInitSuccess) {
      return ETH.gatewayIP();
    } else {
      addLog(LOG_LEVEL_ERROR, F("Call NetworkGatewayIP() only on connected Ethernet!"));
      return IPAddress();
    }
  }
  #endif
  return WiFi.gatewayIP();
}

IPAddress NetworkDnsIP (uint8_t dns_no) {
  #if FEATURE_ETHERNET
  if(active_network_medium == NetworkMedium_t::Ethernet) {
    if(EthEventData.ethInitSuccess) {
      return ETH.dnsIP(dns_no);
    } else {
      addLog(LOG_LEVEL_ERROR, F("Call NetworkDnsIP(uint8_t dns_no) only on connected Ethernet!"));
      return IPAddress();
    }
  }
  #endif
  return WiFi.dnsIP(dns_no);
}

MAC_address NetworkMacAddress() {
  #if FEATURE_ETHERNET
  if(active_network_medium == NetworkMedium_t::Ethernet) {
    return ETHMacAddress();
  }
  #endif
  MAC_address mac;
  WiFi.macAddress(mac.mac);
  return mac;
}

String NetworkGetHostname() {
    #ifdef ESP32
      #if FEATURE_ETHERNET 
      if(Settings.NetworkMedium == NetworkMedium_t::Ethernet && EthEventData.ethInitSuccess) {
        return String(ETH.getHostname());
      }
      #endif
      return String(WiFi.getHostname());
    #else
      return String(WiFi.hostname());
    #endif
}

// ********************************************************************************
// Determine Wifi AP name to set. (also used for mDNS)
// ********************************************************************************
String NetworkGetHostNameFromSettings(bool force_add_unitnr)
{
  if (force_add_unitnr) return Settings.getHostname(true);
  return Settings.getHostname();
}

String NetworkCreateRFCCompliantHostname(bool force_add_unitnr) {
  return createRFCCompliantHostname(NetworkGetHostNameFromSettings(force_add_unitnr));
}

// Create hostname with - instead of spaces
String createRFCCompliantHostname(const String& oldString) {
  String result(oldString);

  // See RFC952.
  // Allowed chars:
  // * letters (a-z, A-Z)
  // * numerals (0-9)
  // * Hyphen (-)
  replaceUnicodeByChar(result, '-');
  for (size_t i = 0; i < result.length(); ++i) {
    const char c = result[i];
    if (!isAlphaNumeric(c)) {
      result[i] = '-';
    }
  }


  // May not start or end with a hyphen
  while (result.startsWith(String('-'))) {
    result = result.substring(1);
  }
  while (result.endsWith(String('-'))) {
    result = result.substring(0, result.length() - 1);
  }

  // May not contain only numerals
  bool onlyNumerals = true;
  for (size_t i = 0; onlyNumerals && i < result.length(); ++i) {
    const char c = result[i];
    if (!isdigit(c)) {
      onlyNumerals = false;
    }
  }
  if (onlyNumerals) {
    result = String(F("ESPEasy-")) + result;
  }

  if (result.length() > 24) {
    result = result.substring(0, 24);
  }

  return result;
}

MAC_address WifiSoftAPmacAddress() {
  MAC_address mac;
  WiFi.softAPmacAddress(mac.mac);
  return mac;
}

MAC_address WifiSTAmacAddress() {
  MAC_address mac;
  WiFi.macAddress(mac.mac);
  return mac;
}

void CheckRunningServices() {
  set_mDNS();
  #ifdef ESP8266
  if (active_network_medium == NetworkMedium_t::WIFI || 
      active_network_medium == NetworkMedium_t::ESPEasyNOW_only) 
  {
    SetWiFiTXpower();
  }
  #endif
}

#if FEATURE_ETHERNET
bool EthFullDuplex()
{
  if (EthEventData.ethInitSuccess)
    return ETH.fullDuplex();
  return false;
}

bool EthLinkUp()
{
  if (EthEventData.ethInitSuccess) {
    #ifdef ESP_IDF_VERSION_MAJOR
    // FIXME TD-er: See: https://github.com/espressif/arduino-esp32/issues/6105
    return EthEventData.EthConnected();
    #else
    return ETH.linkUp();
    #endif
  }
  return false;
}

uint8_t EthLinkSpeed()
{
  if (EthEventData.ethInitSuccess) {
    return ETH.linkSpeed();
  }
  return 0;
}


void stop_eth_dhcps() {
  esp_err_t err = tcpip_adapter_dhcps_stop(TCPIP_ADAPTER_IF_ETH);
  if(err != ESP_OK && err != ESP_ERR_TCPIP_ADAPTER_DHCP_ALREADY_STOPPED){
/*
    if (loglevelActiveFor(LOG_LEVEL_ERROR)) {
      String log = F("ETH   : DHCP server could not be stopped! Error: ");
      log += err;
      addLog(LOG_LEVEL_ERROR, log);
    }
*/
  }
}

#endif
