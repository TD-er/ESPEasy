#ifndef GLOBALS_MESH_SETTINGS_H
#define GLOBALS_MESH_SETTINGS_H

#include "../DataStructs/MeshSettingsStruct.h"

#ifdef USES_WIFI_MESH

class TcpIpMeshBackend ;

extern MeshSettingsStruct MeshSettings;

extern TcpIpMeshBackend* tcpIpNode;

#endif // ifdef USES_WIFI_MESH

#endif // GLOBALS_MESH_SETTINGS_H
