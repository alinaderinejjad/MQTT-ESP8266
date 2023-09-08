
#include "painlessMesh.h"

// AP Const defines
#define   MESH_PREFIX     "Mesh-Eval"
#define   MESH_PASSWORD   "mqtt-mesh"
// Mesh Const defines
#define   MESH_PORT       2193

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;

// Functions Prototype
void sendMessage() ; 

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

void sendMessage() {
  String msg = "Hello from node ";
  msg += mesh.getNodeId();
  mesh.sendBroadcast( msg );
  taskSendMessage.setInterval( random( TASK_SECOND * 1, TASK_SECOND * 5 ));
}

// Recieve callback Routin
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
}

// new connection callback Routin
void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

// change connection callback Routin
void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

// node time adjustion callback Routin-needed for nodes synchronization in mesh protocol
void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}

void setup() {
  Serial.begin(115200);
  Serial.printf("attempting to connect to %s", MESH_PREFIX );
  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();
}



//
// 1. sends a silly message to every node on the mesh at a random time between 1 and 5 seconds
// 2. prints anything it receives to Serial.print
//
//

void loop() {
  //user scheduller is called inside mesh.update
  mesh.update();
}
