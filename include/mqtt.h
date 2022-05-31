#pragma once

extern bool mqtt_enabled;
extern String mqtt_host;
extern String mqtt_username;
extern String mqtt_password;
extern String mqtt_topic;
extern int mqtt_port;

void callback(char* topic, byte* payload, unsigned int length);
void mqtt_init(void);
void mqtt_loop(void);
