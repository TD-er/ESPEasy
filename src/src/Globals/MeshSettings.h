#ifndef GLOBALS_MESH_SETTINGS_H
#define GLOBALS_MESH_SETTINGS_H

#include "../DataStructs/MeshSettingsStruct.h"

#ifdef USES_WIFI_MESH

class ESP8266WiFiMesh;

extern MeshSettingsStruct MeshSettings;

extern ESP8266WiFiMesh* meshNode;

#endif // ifdef USES_WIFI_MESH

#endif // GLOBALS_MESH_SETTINGS_H
