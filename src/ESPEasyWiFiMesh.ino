#ifdef USES_WIFI_MESH

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMesh.h>
#include <TypeConversionFunctions.h>



unsigned int requestNumber = 0;
unsigned int responseNumber = 0;

String manageRequest(const String &request, ESP8266WiFiMesh &meshInstance);
transmission_status_t manageResponse(const String &response, ESP8266WiFiMesh &meshInstance);
void networkFilter(int numberOfNetworks, ESP8266WiFiMesh &meshInstance);


/* Create the mesh node object */
// ESP8266WiFiMesh meshNode = ESP8266WiFiMesh(manageRequest, manageResponse, networkFilter, FPSTR(exampleWiFiPassword), FPSTR(exampleMeshName), "", true);

void createWiFiMeshNode(bool force) {
  // Check if we need to delete meshNode
  if (meshNode != nullptr) {
    bool mustDelete = !MeshSettings.enabled || force;
    if (mustDelete) {
      delete meshNode;
      meshNode = nullptr;
    }
  }
  if (meshNode == nullptr && MeshSettings.enabled) {
    String meshPassword = MeshSettings.MeshPass;
    String meshName = MeshSettings.MeshName;
    String nodeID = MeshSettings.nodeId;
    bool verboseMode = false;
    uint8 meshWiFiChannel = MeshSettings.meshWiFiChannel;
    uint16_t serverPort = MeshSettings.serverPort;
    meshNode = new ESP8266WiFiMesh(
      manageRequest, manageResponse, networkFilter, 
      meshPassword, meshName, nodeID, verboseMode, meshWiFiChannel, serverPort);
  }
}


/**
   Callback for when other nodes send you a request
   @param request The request string received from another node in the mesh
   @param meshInstance The ESP8266WiFiMesh instance that called the function.
   @returns The string to send back to the other node
*/
String manageRequest(const String &request, ESP8266WiFiMesh &meshInstance) {
  // We do not store strings in flash (via F()) in this function.
  // The reason is that the other node will be waiting for our response,
  // so keeping the strings in RAM will give a (small) improvement in response time.
  // Of course, it is advised to adjust this approach based on RAM requirements.

  /* Print out received message */
  Serial.print("Request received: ");
  Serial.println(request);

  /* return a string to send back */
  return ("Hello world response #" + String(responseNumber++) + " from " + meshInstance.getMeshName() + meshInstance.getNodeID() + ".");
}

/**
   Callback for when you get a response from other nodes
   @param response The response string received from another node in the mesh
   @param meshInstance The ESP8266WiFiMesh instance that called the function.
   @returns The status code resulting from the response, as an int
*/
transmission_status_t manageResponse(const String &response, ESP8266WiFiMesh &meshInstance) {
  transmission_status_t statusCode = TS_TRANSMISSION_COMPLETE;

  /* Print out received message */
  Serial.print(F("Request sent: "));
  Serial.println(meshInstance.getMessage());
  Serial.print(F("Response received: "));
  Serial.println(response);

  // Our last request got a response, so time to create a new request.
  meshInstance.setMessage(String(F("Hello world request #")) + String(++requestNumber) + String(F(" from "))
                          + meshInstance.getMeshName() + meshInstance.getNodeID() + String(F(".")));

  // (void)meshInstance; // This is useful to remove a "unused parameter" compiler warning. Does nothing else.
  return statusCode;
}

/**
   Callback used to decide which networks to connect to once a WiFi scan has been completed.
   @param numberOfNetworks The number of networks found in the WiFi scan.
   @param meshInstance The ESP8266WiFiMesh instance that called the function.
*/
void networkFilter(int numberOfNetworks, ESP8266WiFiMesh &meshInstance) {
  for (int networkIndex = 0; networkIndex < numberOfNetworks; ++networkIndex) {
    String currentSSID = WiFi.SSID(networkIndex);
    int meshNameIndex = currentSSID.indexOf(meshInstance.getMeshName());

    /* Connect to any _suitable_ APs which contain meshInstance.getMeshName() */
    if (meshNameIndex >= 0) {
      uint64_t targetNodeID = stringToUint64(currentSSID.substring(meshNameIndex + meshInstance.getMeshName().length()));

      if (targetNodeID < stringToUint64(meshInstance.getNodeID())) {
        ESP8266WiFiMesh::connectionQueue.push_back(NetworkInfo(networkIndex));
      }
    }
  }
}

#endif