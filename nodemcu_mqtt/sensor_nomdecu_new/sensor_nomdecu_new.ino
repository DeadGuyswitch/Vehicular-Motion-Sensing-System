#include "ESP8266WiFi.h"
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_MMA8451.h>
#include <Adafruit_Sensor.h>

String payload;

const char* ssid = "G5_Hotspot";
const char* password = "Bridged000";

const char* clientID = "ESP8266Client1";

const char* mqtt_user = "pi_mqtt";
const char* mqtt_password = "Ion+Maiden-152207x438";

IPAddress mqtt_server = IPAddress(192, 168, 43, 81);

WiFiClient espClient;
PubSubClient mclient(mqtt_server, 1883, espClient);


#define ax_topic "/sensor1/accelerometer"


Adafruit_MMA8451 mma = Adafruit_MMA8451();





volatile float acc_xtop, acc_ytop, acc_ztop;




void setup_wifi() {

  // Connecting to a WiFi network

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}



void reconnect() {
  // Loop until we're reconnected
  //  Serial.println("In reconnect...");

  while (!mclient.connected()) {

    Serial.print("Attempting MQTT connection as ...");
    Serial.println(clientID);


    // Attempt to connect
    if (mclient.connect(clientID, mqtt_user, mqtt_password)) {
      Serial.println("connected");
    }

    else {
      Serial.print("failed, rc=");
      Serial.print(mclient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}



void setup(void) {

  Serial.begin(9600);

  setup_wifi();

  pinMode(LED_BUILTIN, OUTPUT);

  mclient.setServer(mqtt_server, 1883);
  mclient.setClient(espClient);

  if (mclient.connect(clientID, mqtt_user, mqtt_password)) {
    Serial.println("MQTT client connected to server.");
  }
  else {
    Serial.println("Connection Failed");
  }

  Serial.println("Adafruit MMA8451 test!");



  if (! mma.begin()) {
    Serial.println("Couldnt start");
    while (1);
  }
  Serial.println("MMA8451 found!");

  mma.setRange(MMA8451_RANGE_2_G);

  Serial.print("Range = "); Serial.print(2 << mma.getRange());
  Serial.println("G");

}



void loop() {

  mma.read();
  /* Get a new sensor event */
  sensors_event_t event;
  mma.getEvent(&event);

  reconnect();

  acc_xtop = event.acceleration.x;
  acc_ytop = event.acceleration.y;
  acc_ztop = event.acceleration.z;

  payload = String(acc_xtop) + "," + String(acc_ytop) + "," + String(acc_ztop);
  /* Display the results (acceleration is measured in m/s^2) */



  Serial.print("\n acc_X:");
  Serial.print(String(acc_xtop));

  Serial.print(" \n acc_Y:");
  Serial.print(String(acc_ytop));

  Serial.print("\n acc_Z:");
  Serial.print(String(acc_ztop));

  mclient.publish(ax_topic, payload.c_str(), true);

  mclient.loop();
}
