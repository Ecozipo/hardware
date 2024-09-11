#include<Arduino.h>
#include "config.h"
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"
#include <PZEM004Tv30.h>
#include <UUID.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ezButton.h>


/*
 *
 *  Global variables
 *
 */
#define AWS_IOT_PZEM_SENSOR_TOPIC   "esp32/pzem"


#define RELAY_1 25
#define RELAY_2 26
#define RELAY_3 27
#define RELAY_4 14
#define RELAY_5 12
#define RELAY_6 13
#define RELAY_7 32
#define RELAY_8 33


/*
 *
 *  Ez Button
 *
 */

ezButton sw1(5);
ezButton sw2(15);
ezButton sw3(18);
ezButton sw4(19);
ezButton sw5(21);
ezButton sw6(22);
ezButton sw7(23);
ezButton sw8(0);


/*
 *
 * Relay Last state
 *
 *
 */

int reportedRelay1;
int reportedRelay2;
int reportedRelay3;
int reportedRelay4;
int reportedRelay5;
int reportedRelay6;
int reportedRelay7;
int reportedRelay8;


/*
 *
 *  Class Instance
 *
 */

PZEM004Tv30 pzem(Serial2, 16, 17);
UUID uuid;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);


float voltage;
float current;
float power;
float energy;
float freq;
float pf;
String formattedDate;
String AWS_IOT_LED_SHADOW_UPDATE;
String AWS_IOT_LED_SHADOW_ACCEPTED;
String AWS_IOT_LED_SHADOW_DELTA;
String AWS_IOT_LED_SHADOW_GET;


WiFiClientSecure wifi_client = WiFiClientSecure();
MQTTClient mqtt_client = MQTTClient(2048);

void incomingMessageHandler(String& topic, String& payload);
void setupTopics();
void connectAWS();
void publishMessage();
void handleSwitchPress(int relayNumber, int currentSwState);

void setup()
{
    Serial.begin(115200);
    pinMode(RELAY_1, OUTPUT);
    pinMode(RELAY_2, OUTPUT);
    pinMode(RELAY_3, OUTPUT);
    pinMode(RELAY_4, OUTPUT);
    pinMode(RELAY_5, OUTPUT);
    pinMode(RELAY_6, OUTPUT);
    pinMode(RELAY_7, OUTPUT);
    pinMode(RELAY_8, OUTPUT);

    sw1.setDebounceTime(50);

    setupTopics();
    connectAWS();
    timeClient.begin();
    timeClient.setTimeOffset(10800);
}

void loop()
{
    sw1.loop();
    sw2.loop();
    sw3.loop();
    sw4.loop();
    sw5.loop();
    sw6.loop();
    sw7.loop();
    sw8.loop();
    while (!timeClient.update())
    {
        timeClient.forceUpdate();
    }
    formattedDate = timeClient.getFormattedDate();
    voltage = pzem.voltage();
    current = pzem.current();
    power = pzem.power();
    energy = pzem.energy();
    freq = pzem.frequency();
    pf = pzem.pf();
    uuid.generate();
    publishMessage();


    if (sw1.isPressed() || sw1.isReleased())
    {
        int currentSwState = !reportedRelay1;
        handleSwitchPress(1, currentSwState);
    }

    if (sw2.isPressed() || sw2.isReleased())
    {
        int currentSwState = !reportedRelay2;
        handleSwitchPress(2, currentSwState);
    }

    if (sw3.isPressed() || sw3.isReleased())
    {
        int currentSwState = !reportedRelay3;
        handleSwitchPress(3, currentSwState);
    }

    if (sw4.isPressed() || sw4.isReleased())
    {
        int currentSwState = !reportedRelay4;
        handleSwitchPress(4, currentSwState);
    }

    if (sw5.isPressed() || sw5.isReleased())
    {
        int currentSwState = !reportedRelay5;
        handleSwitchPress(5, currentSwState);
    }

    if (sw6.isPressed() || sw6.isReleased())
    {
        int currentSwState = !reportedRelay6;
        handleSwitchPress(6, currentSwState);
    }

    if (sw7.isPressed() || sw7.isReleased())
    {
        int currentSwState = !reportedRelay7;
        handleSwitchPress(7, currentSwState);
    }

    if (sw8.isPressed() || sw8.isReleased())
    {
        int currentSwState = !reportedRelay8;
        handleSwitchPress(8, currentSwState);
    }

    mqtt_client.loop();
    delay(500);
}

/*
 *
 *
 *  FUNCTIONs DEFINITIONS
 *
 */

void setupTopics()
{
    AWS_IOT_LED_SHADOW_UPDATE = "$aws/things/" + String(THINGNAME) + "/shadow/update";
    AWS_IOT_LED_SHADOW_ACCEPTED = "$aws/things/" + String(THINGNAME) + "/shadow/get/accepted";
    AWS_IOT_LED_SHADOW_GET = "$aws/things/" + String(THINGNAME) + "/shadow/get";
    AWS_IOT_LED_SHADOW_DELTA = "$aws/things/" + String(THINGNAME) + "/shadow/update/delta";
}

void connectAWS()
{
    // connect to internet via wifi
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    Serial.println(WIFI_SSID);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println();

    //ESP32 handle TLS Communication to secure connection
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
    mqtt_client.subscribe(AWS_IOT_LED_SHADOW_UPDATE);
    mqtt_client.subscribe(AWS_IOT_LED_SHADOW_DELTA);
    mqtt_client.subscribe(AWS_IOT_LED_SHADOW_ACCEPTED);
    mqtt_client.publish(AWS_IOT_LED_SHADOW_GET, "");
    Serial.println("AWS IoT Connected!");
}

void publishMessage()
{
    StaticJsonDocument<200> doc;
    doc["id"] = uuid;
    doc["created_at"] = formattedDate;
    doc["voltage"] = voltage;
    doc["current"] = current;
    doc["power"] = power;
    doc["energy"] = energy;
    doc["freq"] = freq;
    doc["pf"] = pf;

    char jsonBuffer[512];
    serializeJson(doc, jsonBuffer);

    mqtt_client.publish(AWS_IOT_PZEM_SENSOR_TOPIC, jsonBuffer);
    // Serial.println("message sent !");
}

void incomingMessageHandler(String& topic, String& payload)
{
    Serial.println("Message received!");
    Serial.println("Topic: " + topic);
    Serial.println("Payload: " + payload);

    StaticJsonDocument<400> doc; // Increased size for more relays
    DeserializationError error = deserializeJson(doc, payload);

    if (error)
    {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }

    if (String(topic) == AWS_IOT_LED_SHADOW_DELTA)
    {
        mqtt_client.publish(AWS_IOT_LED_SHADOW_GET, "");
    }

    if (String(topic) == AWS_IOT_LED_SHADOW_ACCEPTED)
    {
        // Safely extract integer values for each relay from the JSON
        int desiredRelay1 = doc["state"]["desired"]["relay_1"].as<int>();
        int desiredRelay2 = doc["state"]["desired"]["relay_2"].as<int>();
        int desiredRelay3 = doc["state"]["desired"]["relay_3"].as<int>();
        int desiredRelay4 = doc["state"]["desired"]["relay_4"].as<int>();
        int desiredRelay5 = doc["state"]["desired"]["relay_5"].as<int>();
        int desiredRelay6 = doc["state"]["desired"]["relay_6"].as<int>();
        int desiredRelay7 = doc["state"]["desired"]["relay_7"].as<int>();
        int desiredRelay8 = doc["state"]["desired"]["relay_8"].as<int>();

        reportedRelay1 = doc["state"]["reported"]["relay_1"].as<int>();
        reportedRelay2 = doc["state"]["reported"]["relay_2"].as<int>();
        reportedRelay3 = doc["state"]["reported"]["relay_3"].as<int>();
        reportedRelay4 = doc["state"]["reported"]["relay_4"].as<int>();
        reportedRelay5 = doc["state"]["reported"]["relay_5"].as<int>();
        reportedRelay6 = doc["state"]["reported"]["relay_6"].as<int>();
        reportedRelay7 = doc["state"]["reported"]["relay_7"].as<int>();
        reportedRelay8 = doc["state"]["reported"]["relay_8"].as<int>();

        // Compare and update relays based on the desired state
        if (desiredRelay1 != reportedRelay1 || desiredRelay2 != reportedRelay2 || desiredRelay3 != reportedRelay3 || (
                desiredRelay4 != reportedRelay4) || (desiredRelay5 != reportedRelay5) || (desiredRelay6 !=
                reportedRelay6)
            || (desiredRelay7 != reportedRelay7) || (desiredRelay8 != reportedRelay8))
        {
            digitalWrite(RELAY_1, desiredRelay1 == 1 ? HIGH : LOW);
            digitalWrite(RELAY_2, desiredRelay2 == 1 ? HIGH : LOW);
            digitalWrite(RELAY_3, desiredRelay3 == 1 ? HIGH : LOW);
            digitalWrite(RELAY_4, desiredRelay4 == 1 ? HIGH : LOW);
            digitalWrite(RELAY_5, desiredRelay5 == 1 ? HIGH : LOW);
            digitalWrite(RELAY_6, desiredRelay6 == 1 ? HIGH : LOW);
            digitalWrite(RELAY_7, desiredRelay7 == 1 ? HIGH : LOW);
            digitalWrite(RELAY_8, desiredRelay8 == 1 ? HIGH : LOW);

            // Create a JSON payload to update the reported state
            StaticJsonDocument<512> updateDoc;
            updateDoc["state"]["reported"]["relay_1"] = desiredRelay1;
            updateDoc["state"]["reported"]["relay_2"] = desiredRelay2;
            updateDoc["state"]["reported"]["relay_3"] = desiredRelay3;
            updateDoc["state"]["reported"]["relay_4"] = desiredRelay4;
            updateDoc["state"]["reported"]["relay_5"] = desiredRelay5;
            updateDoc["state"]["reported"]["relay_6"] = desiredRelay6;
            updateDoc["state"]["reported"]["relay_7"] = desiredRelay7;
            updateDoc["state"]["reported"]["relay_8"] = desiredRelay8;

            char updateJson[512];
            serializeJson(updateDoc, updateJson);

            mqtt_client.publish(AWS_IOT_LED_SHADOW_UPDATE, updateJson);
        }
    }
}

void handleSwitchPress(int relayNumber, int currentSwState)
{
    StaticJsonDocument<512> updateDoc;
    String relayKey = "relay_" + String(relayNumber);
    updateDoc["state"]["desired"][relayKey] = currentSwState;

    char updateJson[512];
    serializeJson(updateDoc, updateJson);

    mqtt_client.publish(AWS_IOT_LED_SHADOW_UPDATE, updateJson);
    Serial.print("desired state for ");
    Serial.print(relayKey);
    Serial.print(" published: ");
    Serial.println(updateJson);
}
