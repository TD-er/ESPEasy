#ifdef USES_WIFI_MESH

#include <ESP8266WiFi.h>
#include <TypeConversionFunctions.h>
#include <FloodingMesh.h>
#include <EspnowMeshBackend.h>
#include <TransmissionOutcome.h>

#include "ESPEasyWiFiMesh.h"
#include "../Globals/MeshSettings.h"


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

  if (delimiterIndex == 0) {
    Serial.print("Message received from STA MAC " + meshInstance.getEspnowMeshBackend().getSenderMac() + ": ");
    Serial.println(message.substring(2, 102));

    String potentialMac = message.substring(2, 14);

    if (potentialMac >= theOneMac) {
      if (potentialMac > theOneMac) {
        theOne    = false;
        theOneMac = potentialMac;
      }

      if (useLED && !theOne) {
        bool ledState = message.charAt(1) == '1';
        digitalWrite(LED_BUILTIN, ledState); // Turn LED on/off (LED_BUILTIN is active low)
      }

      return true;
    } else {
      return false;
    }
  } else if (delimiterIndex > 0) {
    if (meshInstance.getOriginMac() == theOneMac) {
      uint32_t totalBroadcasts = strtoul(message.c_str(), nullptr, 0); // strtoul stops reading input when an invalid character is
                                                                       // discovered.

      // Static variables are only initialized once.
      static uint32_t firstBroadcast = totalBroadcasts;

      if (totalBroadcasts - firstBroadcast >= 100) { // Wait a little to avoid start-up glitches
        static uint32_t missedBroadcasts        = 1; // Starting at one to compensate for initial -1 below.
        static uint32_t previousTotalBroadcasts = totalBroadcasts;
        static uint32_t totalReceivedBroadcasts = 0;
        totalReceivedBroadcasts++;

        missedBroadcasts       += totalBroadcasts - previousTotalBroadcasts - 1; // We expect an increment by 1.
        previousTotalBroadcasts = totalBroadcasts;

        if (totalReceivedBroadcasts % 50 == 0) {
          Serial.println("missed/total: " + String(missedBroadcasts) + '/' + String(totalReceivedBroadcasts));
        }

        if (totalReceivedBroadcasts % 500 == 0) {
          Serial.println("Benchmark message: " + message.substring(0, 100));
        }
      }
    }
  } else {
    // Only show first 100 characters because printing a large String takes a lot of time, which is a bad thing for a callback function.
    // If you need to print the whole String it is better to store it and print it in the loop() later.
    Serial.print("Message with origin " + meshInstance.getOriginMac() + " received: ");
    Serial.println(message.substring(0, 100));
  }

  return true;
}


bool sendFloodingMeshBroadcast(const String& message)
{

  // FIXME TD-er: Must check for length of message.
  if (floodingMesh != nullptr) {
    String tmp_message = String(floodingMesh->metadataDelimiter());
    floodingMesh->broadcast(tmp_message + message);
    floodingMeshDelay(20);
    return true;
  }
  return false;
}


#endif // ifdef USES_WIFI_MESH
