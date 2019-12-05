#include "../Globals/MeshSettings.h"

#include <ESP8266WiFi.h>
#include <TcpIpMeshBackend.h>
#include <FloodingMesh.h>
#include <TypeConversionFunctions.h>


#ifdef USES_WIFI_MESH

MeshSettingsStruct MeshSettings;

TcpIpMeshBackend *tcpIpNode    = nullptr;
FloodingMesh     *floodingMesh = nullptr;

#endif // ifdef USES_WIFI_MESH
