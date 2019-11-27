#ifndef DATASTRUCTS_MESH_SETTINGS_STRUCT_H
#define DATASTRUCTS_MESH_SETTINGS_STRUCT_H

#include "../../ESPEasy_common.h"
#include "../DataStructs/ESPEasyLimits.h"

#ifdef USES_WIFI_MESH

/*********************************************************************************************\
* MeshSettingsStruct
\*********************************************************************************************/
struct MeshSettingsStruct
{
  MeshSettingsStruct();

  void validate();
  uint8_t ProgmemMd5[16] = { 0 }; // crc of the binary that last saved the struct to file.
  uint8_t md5[16]        = { 0 };

  // A custom encryption key is required when using encrypted ESP-NOW transmissions. There is always a default Kok set, but it can be
  // replaced if desired.
  // All ESP-NOW keys below must match in an encrypted connection pair for encrypted communication to be possible.
  uint8_t espnowEncryptionKey[16] = { 0x33, 0x44, 0x33, 0x44, 0x33, 0x44, 0x33, 0x44, // This is the key for encrypting transmissions.
                                      0x33, 0x44, 0x33, 0x44, 0x33, 0x44, 0x32, 0x11
  };

  // This is the secret key used for HMAC during encrypted connection requests.
  uint8_t espnowHashKey[16] = { 0xEF, 0x44, 0x33, 0x0C, 0x33, 0x44, 0xFE, 0x44,
  };

  char MeshName[32];   // The name of the mesh network. 
                       // Used as prefix for the node SSID and to find other network nodes in the
                       // example network filter function.
  char MeshPass[64];   // The WiFi password for the mesh network.
  char nodeId[6];      // The id for this mesh node. Used as suffix for the node SSID. 
                       // If set to "", the id will default to ESP.getChipId().

  uint16_t serverPort = 4011; //  The server port used by the AP of the ESP8266WiFiMesh instance.
  // If multiple APs exist on a single ESP8266, each requires a separate server port.
  // If two AP:s on the same ESP8266 are using the same server port, they will not be able to have both server instances active at the same
  // time.
  // This is managed automatically by the activateAP method.

  uint8  meshWiFiChannel = 1; // The WiFi channel used by the mesh network. Valid values are integers from 1 to 13. Defaults to 1.
  // WARNING: The ESP8266 has only one WiFi channel, and the the station/client mode is always prioritized for channel selection.
  // This can cause problems if several ESP8266WiFiMesh instances exist on the same ESP8266 and use different WiFi channels.
  // In such a case, whenever the station of one ESP8266WiFiMesh instance connects to an AP, it will silently force the
  // WiFi channel of any active AP on the ESP8266 to match that of the station. This will cause disconnects and possibly
  // make it impossible for\ other stations to detect the APs whose WiFi channels have changed.

  byte maxAPStations = 4;  // Max number of connected stations to this nodes AP.  (0 ... 8)

  bool enabled = false;

  byte staticIP[4]; // Static IP address of the node for the mesh network.
  byte bssid[6];    // BSSID of the preferred AP to conect to.

};
#endif // ifdef USES_WIFI_MESH

#endif // DATASTRUCTS_MESH_SETTINGS_STRUCT_H
