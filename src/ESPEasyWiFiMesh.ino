#ifdef USES_WIFI_MESH

#include <ESP8266WiFi.h>
#include <TcpIpMeshBackend.h>
#include <TypeConversionFunctions.h>


unsigned int requestNumber  = 0;
unsigned int responseNumber = 0;

String manageRequest(const String &request, MeshBackendBase &meshInstance);
transmission_status_t manageResponse(const String &response, MeshBackendBase &meshInstance);
void networkFilter(int numberOfNetworks, MeshBackendBase &meshInstance);


// Create the mesh node object
// FIXME TD-er: Support different types of mesh node objects.
void createWiFiMeshNode(bool force) {
  // Check if we need to delete tcpIpNode
  if (tcpIpNode != nullptr) {
    bool mustDelete = !MeshSettings.enabled || force;

    if (mustDelete) {
      delete tcpIpNode;
      tcpIpNode = nullptr;
    }
  }

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
}

/**
   Callback for when other nodes send you a request
   @param request The request string received from another node in the mesh
   @param meshInstance The MeshBackendBase instance that called the function.
   @return The string to send back to the other node. For ESP-NOW, return an empy string ("") if no response should be sent.
*/
String manageRequest(const String &request, MeshBackendBase &meshInstance) {
  // We do not store strings in flash (via F()) in this function.
  // The reason is that the other node will be waiting for our response,
  // so keeping the strings in RAM will give a (small) improvement in response time.
  // Of course, it is advised to adjust this approach based on RAM requirements.

  // To get the actual class of the polymorphic meshInstance, do as follows (meshBackendCast replaces dynamic_cast since RTTI is disabled)
  if (EspnowMeshBackend *espnowInstance = meshBackendCast<EspnowMeshBackend *>(&meshInstance)) {
    String messageEncrypted = espnowInstance->receivedEncryptedMessage() ? ", Encrypted" : ", Unencrypted";
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
  return ("Hello world response #" + String(responseNumber++) + " from " + meshInstance.getMeshName() + meshInstance.getNodeID() + " with AP MAC " + WiFi.softAPmacAddress() + ".");
}

/**
   Callback for when you get a response from other nodes
   @param response The response string received from another node in the mesh
   @param meshInstance The MeshBackendBase instance that called the function.
   @return The status code resulting from the response, as an int
*/
transmission_status_t manageResponse(const String &response, MeshBackendBase &meshInstance) {
  transmission_status_t statusCode = TS_TRANSMISSION_COMPLETE;

  // To get the actual class of the polymorphic meshInstance, do as follows (meshBackendCast replaces dynamic_cast since RTTI is disabled)
  if (EspnowMeshBackend *espnowInstance = meshBackendCast<EspnowMeshBackend *>(&meshInstance)) {
    String messageEncrypted = espnowInstance->receivedEncryptedMessage() ? ", Encrypted" : ", Unencrypted";
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
void networkFilter(int numberOfNetworks, MeshBackendBase &meshInstance) {
  // Note that the network index of a given node may change whenever a new scan is done.
  for (int networkIndex = 0; networkIndex < numberOfNetworks; ++networkIndex) {
    String currentSSID = WiFi.SSID(networkIndex);
    int meshNameIndex = currentSSID.indexOf(meshInstance.getMeshName());

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

#endif // ifdef USES_WIFI_MESH
