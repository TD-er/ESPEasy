#include "../Globals/MeshSettings.h"

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMesh.h>
#include <TypeConversionFunctions.h>


#ifdef USES_WIFI_MESH

MeshSettingsStruct MeshSettings;

extern ESP8266WiFiMesh* meshNode = nullptr;

#endif // ifdef USES_WIFI_MESH
