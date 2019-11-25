#include "../DataStructs/MeshSettingsStruct.h"

#include "../../ESPEasy_common.h"
#include "../DataStructs/ESPEasyLimits.h"

#ifdef USES_WIFI_MESH

MeshSettingsStruct::MeshSettingsStruct() {
  ZERO_FILL(ProgmemMd5);
  ZERO_FILL(md5);
  ZERO_FILL(MeshName);
  ZERO_FILL(MeshPass);
  ZERO_FILL(nodeId);
  ZERO_FILL(staticIP);
}

void MeshSettingsStruct::validate() {
  ZERO_TERMINATE(MeshName);
  ZERO_TERMINATE(MeshPass);
  ZERO_TERMINATE(nodeId);
}

#endif // ifdef USES_WIFI_MESH
