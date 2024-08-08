#include<Arduino.h>
#include "config.h"

#include <WiFiClientSecure.h>
#include <MQTTClient.h> //MQTT Library Source: https://github.com/256dpi/arduino-mqtt

#include <ArduinoJson.h> //ArduinoJson Library Source: https://github.com/bblanchon/ArduinoJson
#include "WiFi.h"

// MQTT topics for the device
#define AWS_IOT_PUBLISH_TOPIC   "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"

#define LED_BUILTIN 2

/*
 *
 *  Local variables
 *
 */
float voltage; // Variable to store voltage value
float freq;


WiFiClientSecure wifi_client = WiFiClientSecure();
MQTTClient mqtt_client = MQTTClient(256); //256 indicates the maximum size for packets being published and received.

void incomingMessageHandler(String& topic, String& payload);
uint32_t t1;

void connectAWS()
{
    //Begin WiFi in station mode
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.println("Connecting to Wi-Fi");

    //Wait for WiFi connection
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println();

    // Configure wifi_client with the correct certificates and keys
    wifi_client.setCACert(AWS_CERT_CA);
    wifi_client.setCertificate(AWS_CERT_CRT);
    wifi_client.setPrivateKey(AWS_CERT_PRIVATE);

    //Connect to AWS IOT Broker. 8883 is the port used for MQTT
    mqtt_client.begin(AWS_IOT_ENDPOINT, 8883, wifi_client);

    //Set action to be taken on incoming messages
    mqtt_client.onMessage(incomingMessageHandler);

    Serial.print("Connecting to AWS IOT");

    //Wait for connection to AWS IoT
    while (!mqtt_client.connect(THINGNAME))
    {
        Serial.print(".");
        delay(100);
    }
    Serial.println();

    if (!mqtt_client.connected())
    {
        Serial.println("AWS IoT Timeout!");
        return;
    }

    //Subscribe to a topic
    mqtt_client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

    Serial.println("AWS IoT Connected!");
}

void publishMessage()
{
    //Create a JSON document of size 200 bytes, and populate it
    //See https://arduinojson.org/
    StaticJsonDocument < 200 > doc;
    doc["id"] = random(1000);
    doc["created_at"] = millis() - t1;
    doc["voltage"] = voltage;
    doc["Freq"] = freq;
    char jsonBuffer[512];
    serializeJson(doc, jsonBuffer); // print to mqtt_client

    //Publish to the topic
    mqtt_client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
    Serial.println("message sent !");
}

void incomingMessageHandler(String& topic, String& payload)
{
    Serial.println("Message received!");
    Serial.println("Topic: " + topic);
    Serial.println("Payload: " + payload);

    // Parse the JSON payload
    StaticJsonDocument < 200 > doc; // Adjust size according to your needs
    DeserializationError error = deserializeJson(doc, payload);

    // Check if parsing succeeds
    if (error)
    {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }

    // Extract the value of "message"
    const char* message = doc["message"]; // "ON" or "OFF"

    // Compare the message and act accordingly
    if (strcmp(message, "ON") == 0)
    {
        digitalWrite(LED_BUILTIN, HIGH);
        Serial.println("LED turned ON");
    }
    else if (strcmp(message, "OFF") == 0)
    {
        digitalWrite(LED_BUILTIN, LOW);
        Serial.println("LED turned OFF");
    }
}

void setup()
{
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
    t1 = millis();
    connectAWS();
}

void loop()
{
    publishMessage();
    mqtt_client.loop();
    delay(4000);
}
