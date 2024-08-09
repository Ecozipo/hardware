#include<Arduino.h>
#include "config.h"
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"
#include <PZEM004Tv30.h>
/*
 *
 *  Global variables
 *
 */
#define AWS_IOT_PUBLISH_TOPIC   "esp32/pzem"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/led"
#define LED_BUILTIN 2

PZEM004Tv30 pzem(Serial2, 16, 17);
float voltage;
float current;
float power;
float energy;
float freq;
float pf;


WiFiClientSecure wifi_client = WiFiClientSecure();
MQTTClient mqtt_client = MQTTClient(256);

void incomingMessageHandler(String& topic, String& payload);
uint32_t t1;

void connectAWS()
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.println("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println();

    wifi_client.setCACert(AWS_CERT_CA);
    wifi_client.setCertificate(AWS_CERT_CRT);
    wifi_client.setPrivateKey(AWS_CERT_PRIVATE);

    mqtt_client.begin(AWS_IOT_ENDPOINT, 8883, wifi_client);
    mqtt_client.onMessage(incomingMessageHandler);

    Serial.print("Connecting to AWS IOT");
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
    mqtt_client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
    Serial.println("AWS IoT Connected!");
}

void publishMessage()
{
    StaticJsonDocument<200> doc;
    doc["id"] = random(1000);
    doc["created_at"] = millis() - t1;
    doc["voltage"] = voltage;
    doc["current"] = current;
    doc["energy"] = energy;
    doc["freq"] = freq;
    doc["pf"] = pf;

    char jsonBuffer[512];
    serializeJson(doc, jsonBuffer);

    mqtt_client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
    Serial.println("message sent !");
}

void incomingMessageHandler(String& topic, String& payload)
{
    Serial.println("Message received!");
    Serial.println("Topic: " + topic);
    Serial.println("Payload: " + payload);

    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (error)
    {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }

    const char* message = doc["message"];

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
    voltage = pzem.voltage();
    current = pzem.current();
    power = pzem.power();
    energy = pzem.energy();
    freq = pzem.frequency();
    pf = pzem.pf();

    publishMessage();
    mqtt_client.loop();
    delay(4000);
}
