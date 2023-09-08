#include <painlessMesh.h>

#define   STATION_SSID     "Red Phoenix"
#define   STATION_PASSWORD "PhoenixPhoenix"


#define   MESH_PREFIX     "enigmamachine"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555
#define   mesh_channel  6


#define HOSTNAME "MQTT_Handler"


Scheduler userScheduler; 
painlessMesh  localmesh;
WiFiClient wificli;


void sendMessage() ;

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

void sendMessage() {
  String msg = "Hello from node ";
  msg += localmesh.getNodeId();
  localmesh.sendBroadcast( msg );
  taskSendMessage.setInterval( random( TASK_SECOND * 1, TASK_SECOND * 5 ));
}


void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());

}






void setup() {

  Serial.begin(115200);
  Serial.printf("started");

    localmesh.init( MESH_PREFIX, MESH_PASSWORD,&userScheduler, MESH_PORT, WIFI_AP_STA, mesh_channel );
    localmesh.onReceive(&receivedCallback);
    localmesh.stationManual(STATION_SSID, STATION_PASSWORD);
    
    //uncomment bellow line for the bridge node 
    //localmesh.setHostname(HOSTNAME);


    //uncomment bellow line for the bridge node
    localmesh.setRoot(true);
    
    localmesh.setContainsRoot(true);


  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();




}

void loop() {

localmesh.update();

}




