#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// AP Const defines
const char *ssid = "Red Phoenix"; // The Acess Point SSID
const char *password = "PhoenixPhoenix";  // AP Credentials

// MQTT Broker defines
const char *mqtt_broker = "broker.emqx.io"; //The MQTT Broker Will Provide you with this
const char *topic = "MQTT-Remote-Sense";  //Name of the Topic 
const char *mqtt_username = "ESP8266-TT"; //you should define a a username in the Broker Platform and Give Propper Permissions 
const char *mqtt_password = "AVANGARD"; //you should also set the password for the username
const int mqtt_port = 1883; //The MQTT Broker Will Provide you with this


WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  // Init the Baud Rate
  Serial.begin(115200);
  // Wifi Connection procedure
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.println("Trying to Connect to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  // MQTT Broker Connection config procedure-Setting url,port and callback routin
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  while (!client.connected()) {
      String client_id = "ESP8266-TT-";
      client_id += String(WiFi.macAddress());
      Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
      if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
          Serial.println("Public emqx mqtt broker connected");
      } else {
          Serial.print("failed with state ");
          Serial.print(client.state());
          delay(2000);
      }
  }
  //Toppic Subscription
  client.publish(topic, "ESP8266-TT Back Online");
  client.subscribe(topic);
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
      Serial.print((char) payload[i]);
  }
  Serial.println();
  Serial.println("-----------------------");
}

void loop() {
  client.loop();
}
