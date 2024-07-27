#include <Arduino.h>
#include "secrets.h"
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"

/*

	Global variables

*/
#define AWS_IOT_PUBLISH_TOPIC "esp32/esp32-to-aws"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/aws-to-esp32"
#define PUBLISH_INTERVAL 4000  // 4 seconds
unsigned long lastPublishTime = 0;

/*

	Object definitions

*/

WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(256);

/*

	Functions declarations

*/

void connectToAWS();
void sendToAWS();
void messageHandler(String &topic, String &payload);

/*

	Hardware initiations

*/
void setup() {
 // Serial monitor startup with 9600MHz Frequency
  Serial.begin(9600);

 // Wifi connection mode
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("ESP32 connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

// Connection to AWS IoT Core via secrets.h
  connectToAWS();
}


/*

	The Main loop of the application

*/

void loop() {
  client.loop();

  if (millis() - lastPublishTime > PUBLISH_INTERVAL) {
    sendToAWS();
    lastPublishTime = millis();
  }
}

/*

	Functions Implementations

*/

void connectToAWS() {
  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.begin(AWS_IOT_ENDPOINT, 8883, net);

  // Create a handler for incoming messages
  client.onMessage(messageHandler);

  Serial.print("ESP32 connecting to AWS IOT");

  while (!client.connect(THINGNAME)) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  if (!client.connected()) {
    Serial.println("ESP32 - AWS IoT Timeout!");
    return;
  }

  // Subscribe to a topic, the incoming messages are processed by messageHandler() function
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

  Serial.println("ESP32  - AWS IoT Connected!");
}


void sendToAWS() {
  StaticJsonDocument<200> message;
  message["timestamp"] = millis();
  message["data"] = analogRead(0); // Or you can read data from other sensors
  char messageBuffer[512];
  serializeJson(message, messageBuffer);  // print to client

  client.publish(AWS_IOT_PUBLISH_TOPIC, messageBuffer);

  Serial.println("sent:");
  Serial.print("- topic: ");
  Serial.println(AWS_IOT_PUBLISH_TOPIC);
  Serial.print("- payload:");
  Serial.println(messageBuffer);
}

void messageHandler(String &topic, String &payload) {
  Serial.println("received:");
  Serial.println("- topic: " + topic);
  Serial.println("- payload:");
  Serial.println(payload);

  // You can process the incoming data as json object, then control something
  /*
  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  const char* message = doc["message"];
  */
}

