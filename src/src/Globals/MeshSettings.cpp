#include "../Globals/MeshSettings.h"

#ifdef USES_WIFI_MESH

#include <ESP8266WiFi.h>
#include <FloodingMesh.h>
#include <TypeConversionFunctions.h>



MeshSettingsStruct MeshSettings;

FloodingMesh     *floodingMesh = nullptr;

#endif // ifdef USES_WIFI_MESH
