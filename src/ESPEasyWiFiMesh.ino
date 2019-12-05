#ifdef USES_WIFI_MESH

#include <ESP8266WiFi.h>
#include <TcpIpMeshBackend.h>
#include <TypeConversionFunctions.h>


unsigned int requestNumber  = 0;
unsigned int responseNumber = 0;

bool   theOne    = true;
String theOneMac = "";

bool useLED = false; // Change this to true if you wish the onboard LED to mark The One.


// TcpIpMesh
String                manageRequest(const String   & request,
                                    MeshBackendBase& meshInstance);
transmission_status_t manageResponse(const String   & response,
                                     MeshBackendBase& meshInstance);
void                  networkFilter(int              numberOfNetworks,
                                    MeshBackendBase& meshInstance);

// FloodingMesh
bool meshMessageHandler(String      & message,
                        FloodingMesh& meshInstance);

bool meshActive() {
  return tcpIpNode != nullptr || floodingMesh != nullptr;
}

void deleteWiFiMeshNode() {
  if (tcpIpNode != nullptr) {
    delete tcpIpNode;
    tcpIpNode = nullptr;
  }

  if (floodingMesh != nullptr) {
    // FIXME TD-er: Store the state of the existing mesh.
    delete floodingMesh;
    floodingMesh = nullptr;
  }
}

// Create the mesh node object
// FIXME TD-er: Support different types of mesh node objects.
void createWiFiMeshNode(bool force) {
  // Check if we need to delete tcpIpNode
  if (!MeshSettings.enabled || force) {
    deleteWiFiMeshNode();
  }

  // FIXME TD-er: Must store the mesh state and use that for (re-)initialization of the mesh when waking.

  /*
     if ((tcpIpNode == nullptr) && MeshSettings.enabled) {
      String   meshPassword    = MeshSettings.MeshPass;
      String   meshName        = MeshSettings.MeshName;
      String   nodeID          = MeshSettings.nodeId;
      bool     verboseMode     = false;
      uint8    meshWiFiChannel = MeshSettings.meshWiFiChannel;
      uint16_t serverPort      = MeshSettings.serverPort;
      tcpIpNode = new TcpIpMeshBackend(
        manageRequest, manageResponse, networkFilter,
        meshPassword, meshName, nodeID, verboseMode, meshWiFiChannel, serverPort);
     }
   */

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

// TcpIpMesh *********************************************

/**
   Callback for when other nodes send you a request
   @param request The request string received from another node in the mesh
   @param meshInstance The MeshBackendBase instance that called the function.
   @return The string to send back to the other node. For ESP-NOW, return an empy string ("") if no response should be sent.
 */
String manageRequest(const String& request, MeshBackendBase& meshInstance) {
  // We do not store strings in flash (via F()) in this function.
  // The reason is that the other node will be waiting for our response,
  // so keeping the strings in RAM will give a (small) improvement in response time.
  // Of course, it is advised to adjust this approach based on RAM requirements.

  // To get the actual class of the polymorphic meshInstance, do as follows (meshBackendCast replaces dynamic_cast since RTTI is disabled)
  if (EspnowMeshBackend *espnowInstance = meshBackendCast<EspnowMeshBackend *>(&meshInstance)) {
    String messageEncrypted = espnowInstance->receivedEncryptedTransmission() ? ", Encrypted" : ", Unencrypted";
    Serial.print("ESP-NOW (" + espnowInstance->getSenderMac() + messageEncrypted + "): ");
  } else if (TcpIpMeshBackend *tcpIpInstance = meshBackendCast<TcpIpMeshBackend *>(&meshInstance)) {
    (void)tcpIpInstance; // This is useful to remove a "unused parameter" compiler warning. Does nothing else.
    Serial.print("TCP/IP: ");
  } else {
    Serial.print("UNKNOWN!: ");
  }

  /* Print out received message */

  // Only show first 100 characters because printing a large String takes a lot of time, which is a bad thing for a callback function.
  // If you need to print the whole String it is better to store it and print it in the loop() later.
  Serial.print("Request received: ");
  Serial.println(request.substring(0, 100));

  /* return a string to send back */
  return "Hello world response #" + String(responseNumber++) + " from " + meshInstance.getMeshName() + meshInstance.getNodeID() +
         " with AP MAC " + WiFi.softAPmacAddress() + ".";
}

/**
   Callback for when you get a response from other nodes
   @param response The response string received from another node in the mesh
   @param meshInstance The MeshBackendBase instance that called the function.
   @return The status code resulting from the response, as an int
 */
transmission_status_t manageResponse(const String& response, MeshBackendBase& meshInstance) {
  transmission_status_t statusCode = TS_TRANSMISSION_COMPLETE;

  // To get the actual class of the polymorphic meshInstance, do as follows (meshBackendCast replaces dynamic_cast since RTTI is disabled)
  if (EspnowMeshBackend *espnowInstance = meshBackendCast<EspnowMeshBackend *>(&meshInstance)) {
    String messageEncrypted = espnowInstance->receivedEncryptedTransmission() ? ", Encrypted" : ", Unencrypted";
    Serial.print("ESP-NOW (" + espnowInstance->getSenderMac() + messageEncrypted + "): ");
  } else if (TcpIpMeshBackend *tcpIpInstance = meshBackendCast<TcpIpMeshBackend *>(&meshInstance)) {
    Serial.print("TCP/IP: ");

    // Getting the sent message like this will work as long as ONLY(!) TCP/IP is used.
    // With TCP/IP the response will follow immediately after the request, so the stored message will not have changed.
    // With ESP-NOW there is no guarantee when or if a response will show up, it can happen before or after the stored message is changed.
    // So for ESP-NOW, adding unique identifiers in the response and request is required to associate a response with a request.
    Serial.print(F("Request sent: "));
    Serial.println(tcpIpInstance->getCurrentMessage().substring(0, 100));
  } else {
    Serial.print("UNKNOWN!: ");
  }

  /* Print out received message */

  // Only show first 100 characters because printing a large String takes a lot of time, which is a bad thing for a callback function.
  // If you need to print the whole String it is better to store it and print it in the loop() later.
  Serial.print(F("Response received: "));
  Serial.println(response.substring(0, 100));

  return statusCode;
}

/**
   Callback used to decide which networks to connect to once a WiFi scan has been completed.
   @param numberOfNetworks The number of networks found in the WiFi scan.
   @param meshInstance The MeshBackendBase instance that called the function.
 */
void networkFilter(int numberOfNetworks, MeshBackendBase& meshInstance) {
  // Note that the network index of a given node may change whenever a new scan is done.
  for (int networkIndex = 0; networkIndex < numberOfNetworks; ++networkIndex) {
    String currentSSID   = WiFi.SSID(networkIndex);
    int    meshNameIndex = currentSSID.indexOf(meshInstance.getMeshName());

    /* Connect to any _suitable_ APs which contain meshInstance.getMeshName() */
    if (meshNameIndex >= 0) {
      uint64_t targetNodeID = stringToUint64(currentSSID.substring(meshNameIndex + meshInstance.getMeshName().length()));

      if (targetNodeID < stringToUint64(meshInstance.getNodeID())) {
        if (EspnowMeshBackend *espnowInstance = meshBackendCast<EspnowMeshBackend *>(&meshInstance)) {
          espnowInstance->connectionQueue().push_back(networkIndex);
        } else if (TcpIpMeshBackend *tcpIpInstance = meshBackendCast<TcpIpMeshBackend *>(&meshInstance)) {
          tcpIpInstance->connectionQueue().push_back(networkIndex);
        } else {
          Serial.println(String(F("Invalid mesh backend!")));
        }
      }
    }
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

#endif // ifdef USES_WIFI_MESH
