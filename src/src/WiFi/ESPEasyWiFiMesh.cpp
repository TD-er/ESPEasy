#ifdef USES_WIFI_MESH

#include <ESP8266WiFi.h>
#include <TypeConversionFunctions.h>
#include <FloodingMesh.h>
#include <EspnowMeshBackend.h>
#include <TransmissionOutcome.h>

#include "ESPEasyWiFiMesh.h"
#include "../Globals/MeshSettings.h"
#include "../DataStructs/EventValueSource.h"
#include "../../ESPEasy_Log.h"
#include "../../ESPEasy_fdwdecl.h"



unsigned int requestNumber  = 0;
unsigned int responseNumber = 0;

bool   theOne    = true;
String theOneMac = "";

bool useLED = false; // Change this to true if you wish the onboard LED to mark The One.

// FloodingMesh
bool meshMessageHandler(String      & message,
                        FloodingMesh& meshInstance);

bool meshActive() {
  return floodingMesh != nullptr;
}

void meshDelay(uint32_t time) {
  // The floodingMeshDelay() method performs all the background operations for the FloodingMesh (via FloodingMesh::performMeshMaintenance()).
  // It is recommended to place one of these methods in the beginning of the loop(), unless there is a need to put them elsewhere.
  // Among other things, the method cleans up old ESP-NOW log entries (freeing up RAM) and forwards received mesh messages.
  // Note that depending on the amount of messages to forward and their length, this method can take tens or even hundreds of milliseconds to complete.
  // More intense transmission activity and less frequent calls to performMeshMaintenance will likely cause the method to take longer to complete, so plan accordingly.
  // The maintenance methods should not be used inside the meshMessageHandler callback, since they can alter the mesh node state. The framework will alert you during runtime if you make this mistake.
  floodingMeshDelay(time);
}

void deleteWiFiMeshNode() {
  if (floodingMesh != nullptr) {
    // FIXME TD-er: Store the state of the existing mesh.
    delete floodingMesh;
    floodingMesh = nullptr;
  }
}

// Create the mesh node object
// FIXME TD-er: Support different types of mesh node objects.
void createWiFiMeshNode(bool force) {
  if (!MeshSettings.enabled || force) {
    deleteWiFiMeshNode();
  }

  // FIXME TD-er: Must store the mesh state and use that for (re-)initialization of the mesh when waking.

  if ((floodingMesh == nullptr) && MeshSettings.enabled) {
    String meshPassword    = MeshSettings.MeshPass;
    String meshName        = MeshSettings.MeshName;
    String nodeID          = MeshSettings.nodeId;
    bool   verboseMode     = false;
    uint8  meshWiFiChannel = MeshSettings.meshWiFiChannel;
    floodingMesh = new FloodingMesh(
      meshMessageHandler, meshPassword, MeshSettings.espnowEncryptedConnectionKey, MeshSettings.espnowHashKey,
      meshName, uint64ToString(ESP.getChipId()), verboseMode, meshWiFiChannel);
  }

  if (floodingMesh != nullptr) {
    floodingMesh->setMessageLogSize(10); // FIXME TD-er: Must make this a setting.
    floodingMesh->begin();
    floodingMesh->activateAP();

    uint8_t apMacArray[6] { 0 };
    theOneMac = macToString(WiFi.softAPmacAddress(apMacArray));
  }
}



// FloodingMesh *******************************

/**
   Callback for when a message is received from the mesh network.

   @param message The message String received from the mesh.
                  Modifications to this String are passed on when the message is forwarded from this node to other nodes.
                  However, the forwarded message will still use the same messageID.
                  Thus it will not be sent to nodes that have already received this messageID.
                  If you want to send a new message to the whole network, use a new broadcast from within the loop() instead.
   @param meshInstance The FloodingMesh instance that received the message.
   @return True if this node should forward the received message to other nodes. False otherwise.
 */
bool meshMessageHandler(String& message, FloodingMesh& meshInstance) {
  int32_t delimiterIndex = message.indexOf(meshInstance.metadataDelimiter());
  if (delimiterIndex == -1) return false;
  int intval;
  if (!validIntFromString(message.substring(0, delimiterIndex), intval)) {
    String log;
    log = F("Mesh invalid messageType: ");
    log += message.substring(0, delimiterIndex);
    addLog(LOG_LEVEL_ERROR, log);
    return false;
  }
  mesh_message_type_t messageType = static_cast<mesh_message_type_t>(intval);

  if (loglevelActiveFor(LOG_LEVEL_INFO)) {
    String log = F("Message received from STA MAC ");
    log += meshInstance.getEspnowMeshBackend().getSenderMac();
    log += ": ";
    log += message.substring(delimiterIndex + 1, delimiterIndex + 100);
    addLog(LOG_LEVEL_INFO, log);
  }

  bool handled = false;

  switch (messageType) {
    case MESH_COMMAND:
    {
      handled = ExecuteCommand_internal(VALUE_SOURCE_MESH, message.substring(delimiterIndex + 1).c_str());
    }
    break;
    case MESH_HOSTINFO:
    break;
  }
  return !handled;
}


bool sendFloodingMeshBroadcast(mesh_message_type_t messageType, const String& message)
{

  // FIXME TD-er: Must check for length of message.
  if (floodingMesh != nullptr) {
    String tmp_message = String(messageType);
    tmp_message += String(floodingMesh->metadataDelimiter());
    floodingMesh->broadcast(tmp_message + message);
    floodingMeshDelay(20);
    addLog(LOG_LEVEL_INFO, String(F("FloodMesh: ")) + message);
    return true;
  }
  return false;
}


#endif // ifdef USES_WIFI_MESH
