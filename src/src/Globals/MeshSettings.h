#ifndef GLOBALS_MESH_SETTINGS_H
#define GLOBALS_MESH_SETTINGS_H

#include "../DataStructs/MeshSettingsStruct.h"

#ifdef USES_WIFI_MESH

class TcpIpMeshBackend;
class FloodingMesh;

extern MeshSettingsStruct MeshSettings;

extern TcpIpMeshBackend *tcpIpNode;
extern FloodingMesh     *floodingMesh;

#endif // ifdef USES_WIFI_MESH

#endif // GLOBALS_MESH_SETTINGS_H
