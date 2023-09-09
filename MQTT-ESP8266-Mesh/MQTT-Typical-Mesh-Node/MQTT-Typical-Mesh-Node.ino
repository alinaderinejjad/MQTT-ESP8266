#include <Arduino.h>
#include <painlessMesh.h>


#define   MESH_PREFIX     "Mesh of things!"
#define   MESH_PASSWORD   "elasticity"
#define   MESH_PORT       2193
#define   channel       11  //This should match the AP channel-for more reliable operation the AP must be run in fixed channel mode



// Prototypes
void receivedCallback( const uint32_t &from, const String &msg );


//Instantiation
painlessMesh  localmesh;

void setup() {
  Serial.begin(115200);

  //mesh setup
  localmesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION );  // set before init() so that you can see startup messages
  localmesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT, WIFI_AP_STA, channel );
  localmesh.onReceive(&receivedCallback);


  //Setting the Bridge Node to root to quicken mesh formation
  //localmesh.setRoot(true);
  //When a root node exists in the Mesh, other nodes should be aware of it.
  localmesh.setContainsRoot(true);
}

void loop() 
{
  localmesh.update();
}


//callback routine for mesh
void receivedCallback( const uint32_t &from, const String &msg ) {
  Serial.printf("bridge: Received from %u msg=%s\n", from, msg.c_str());

}


