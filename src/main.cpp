#include <Arduino.h>
#include <LittleFS.h>
#include <WiFiSettings.h>
#include <ArduinoOTA.h>

#include "anim.h"
#include "ntp.h"
#include "VFD.h"
#include "mqtt.h"

#define WIFI_TIMEOUT_SEC 15
#define ARRAY_LEN(X) (sizeof (X) / sizeof (X)[0])

static void portal(void)
{
  anim_string(anim_wave, ARRAY_LEN(anim_wave), "   config   ", Vfd);
  ArduinoOTA.handle();
}

static int connecting(void)
{
  anim_string(anim_wave, ARRAY_LEN(anim_wave), " connecting ", Vfd);
  return 50;
}

static void setup_ota(void)
{
  ArduinoOTA.setHostname(WiFiSettings.hostname.c_str());
  ArduinoOTA.setPassword(WiFiSettings.password.c_str());
  ArduinoOTA.begin();
}

static void connected(void)
{
  anim_play(anim_init, 0, 4, Vfd);
  setup_ota();
  anim_play(anim_init, 4, 5, Vfd);
  ntp_init();
  anim_play(anim_init, 9, 11, Vfd);
  mqtt_init();
}

void setup(void)
{
  Serial.begin(115200);
  LittleFS.begin();

  pinMode(D3, OUTPUT);
  digitalWrite(D3, LOW);

  Vfd.init();
  Vfd.write("            ");

  ntp_utc_offset_sec = WiFiSettings.integer("utc_offset_sec", 1 * 60 * 60, "UTC offset (seconds)");
  ntp_update_interval_sec = WiFiSettings.integer("update_interval_sec", 24 * 60 * 60, "NTP update interval (seconds)");
  
  mqtt_enabled = WiFiSettings.checkbox("mqtt_enabled", false);
  mqtt_host = WiFiSettings.string("mqtt_host", "mqtt.example.org");
  mqtt_username = WiFiSettings.string("mqtt_username", "username");
  mqtt_password = WiFiSettings.string("mqtt_password", "password");
  mqtt_topic = WiFiSettings.string("mqtt_topic", "topic");
  mqtt_port = WiFiSettings.integer("mqtt_port", 0, 65535, 1883);

  WiFiSettings.onWaitLoop = connecting;
  WiFiSettings.onSuccess = connected;
  WiFiSettings.onPortal = setup_ota;
  WiFiSettings.onPortalWaitLoop = portal;
  WiFiSettings.connect(true, WIFI_TIMEOUT_SEC);
}

void loop(void)
{
  char b[13];
  ntp_fmt(b);
  Vfd.write(b);
  ArduinoOTA.handle();
  mqtt_loop();
  delay(100);
}
