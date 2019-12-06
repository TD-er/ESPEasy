#ifndef WIFI_ESPEASYWIFIMESH_H
#define WIFI_ESPEASYWIFIMESH_H

#include "../../ESPEasy_common.h"

class FloodingMesh;

bool meshMessageHandler(String      & message,
                        FloodingMesh& meshInstance);

bool meshActive();

void meshDelay(uint32_t time);

void deleteWiFiMeshNode();

void createWiFiMeshNode(bool force);

bool meshMessageHandler(String& message, FloodingMesh& meshInstance);

bool sendFloodingMeshBroadcast(const String& message);

#endif // WIFI_ESPEASYWIFIMESH_H