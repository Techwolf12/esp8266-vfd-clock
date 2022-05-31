#include <Arduino.h>
#include <VFD.h>
#include <PubSubClient.h>
#include <WiFiClient.h>

#include <mqtt.h>

bool mqtt_enabled;
String mqtt_host;
String mqtt_username;
String mqtt_password;
String mqtt_topic;
int mqtt_port;

WiFiClient espClient;
PubSubClient mqttClient(espClient);

void callback(char* topic, byte* payload, unsigned int length)
{
    // Handle messages from MQTT subscriptions

        String message = "";
        //TODO: This can be handled better
        for (unsigned int i = 0; i < length; i++) {
            message += (char)payload[i];
        }
        char buffer[length+1];
        message.toCharArray(buffer, length);
        Vfd.write(buffer);
        //TODO: If string too big for screen, scroll
        delay(length * 1000);
    
}

void mqtt_init()
{
    if (mqtt_enabled) {
        mqttClient.setServer(mqtt_host.c_str(), mqtt_port);
        mqttClient.setCallback(callback);
        if (mqttClient.connect("VFD-Clock", mqtt_username.c_str(), mqtt_password.c_str(), mqtt_topic.c_str(), 0, false, "dead")) {
            mqttClient.publish(mqtt_topic.c_str(), "active");
            mqttClient.subscribe(mqtt_topic.c_str());
        }
    }
    
}

void mqtt_loop()
{
    if (mqtt_enabled) {
        mqttClient.loop();
    }
}

