#include <Arduino.h>
#include <painlessMesh.h>
#include <PubSubClient.h>
#include <WiFiClient.h>

#define   MESH_PREFIX     "Mesh of things!"
#define   MESH_PASSWORD   "elasticity"
#define   MESH_PORT       2193
#define   channel       11  //This should match the AP channel-for more reliable operation the AP must be run in fixed channel mode



#define   STATION_SSID     "s@m@^68!"// The Acess Point SSID
#define   STATION_PASSWORD "4oo1hajkhodadad^m@hb00beh*"// AP Credentials

#define HOSTNAME "MQTT Bridge Node"


const char *mqtt_username = "ESP8266-TT"; //you should define a username in the Broker Platform and Give Propper Permissions 
const char *mqtt_password = "AVANGARD";//you should also set the password for the username



// Prototypes
void receivedCallback( const uint32_t &from, const String &msg );
void mqttCallback(char* topic, byte* payload, unsigned int length);
IPAddress getlocalIP();

//creating an object
IPAddress myIP(0,0,0,0);


//the URL is broker.emqx.io
//The IP is extracted with nslookup command in Windows cmd
IPAddress mqttBroker(54, 244, 173, 190);

//Instantiation
painlessMesh  localmesh;
WiFiClient wificli;
PubSubClient mqttClient(mqttBroker, 1883, mqttCallback, wificli);

void setup() {
  Serial.begin(115200);

  //mesh setup
  localmesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION );  // set before init() so that you can see startup messages
  localmesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT, WIFI_AP_STA, channel );
  localmesh.onReceive(&receivedCallback);
  //Connecting to the AP
  localmesh.stationManual(STATION_SSID, STATION_PASSWORD);
  //Setting the name for this Node
  localmesh.setHostname(HOSTNAME);

  //Setting the Bridge Node to root to quicken mesh formation
  localmesh.setRoot(true);
  //When a root node exists in the Mesh, other nodes should be aware of it.
  localmesh.setContainsRoot(true);
}

void loop() {
  localmesh.update();
  mqttClient.loop();

  if(myIP != getlocalIP()){
    myIP = getlocalIP();
    Serial.println("My IP is " + myIP.toString());

//to reconnect if the connection is lost
    if (mqttClient.connect("painlessMeshClient")) {
      mqttClient.publish("painlessMesh/from/gateway","Ready!");
      mqttClient.subscribe("painlessMesh/to/#");
    } 
  }
}


//callback routine for mesh
void receivedCallback( const uint32_t &from, const String &msg ) {
  Serial.printf("bridge: Received from %u msg=%s\n", from, msg.c_str());
  String topic = "painlessMesh/from/" + String(from);
  mqttClient.publish(topic.c_str(), msg.c_str());
}


//callback routine for mqttclient
void mqttCallback(char* topic, uint8_t* payload, unsigned int length) {
  char* cleanPayload = (char*)malloc(length+1);
  memcpy(cleanPayload, payload, length);
  cleanPayload[length] = '\0';
  String msg = String(cleanPayload);
  free(cleanPayload);

  String targetStr = String(topic).substring(16);

  if(targetStr == "gateway")
  {
    if(msg == "getNodes")
    {
      auto nodes = localmesh.getNodeList(true);
      String str;
      for (auto &&id : nodes)
        str += String(id) + String(" ");
      mqttClient.publish("painlessMesh/from/gateway", str.c_str());
    }
  }
  else if(targetStr == "broadcast") 
  {
    localmesh.sendBroadcast(msg);
  }
  else
  {
    uint32_t target = strtoul(targetStr.c_str(), NULL, 10);
    if(localmesh.isConnected(target))
    {
      localmesh.sendSingle(target, msg);
    }
    else
    {
      mqttClient.publish("painlessMesh/from/gateway", "Client not connected!");
    }
  }
}


//receive the DHCP leased IP for the Node
IPAddress getlocalIP() {
  return IPAddress(localmesh.getStationIP());
}
