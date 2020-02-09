#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>
#include "ESP8266httpUpdate.h"

#define USE_SERIAL Serial

#ifndef APSSID
#define APSSID "Villa Z40"
#define APPSK  "icyhat740"
#endif

ESP8266WiFiMulti WiFiMulti;

String host_name = "http://sensesmart.in/blink.bin"; // "https://us-central1-sense-smart.cloudfunctions.net/update?simple=yes"; //

void setup() {

  USE_SERIAL.begin(9600);
  //USE_SERIAL.setDebugOutput(true);

  USE_SERIAL.println();
  USE_SERIAL.println();
  USE_SERIAL.println();

  for (uint8_t t = 4; t > 0; t--) {
    USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
    USE_SERIAL.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(APSSID, APPSK);

  Serial.print("x-ESP8266-Chip-ID : "); Serial.println(String(ESP.getChipId()));
  Serial.print("x-ESP8266-STA-MAC : "); Serial.println(WiFi.macAddress());
  Serial.print("x-ESP8266-AP-MAC : "); Serial.println(WiFi.softAPmacAddress());
  Serial.print("x-ESP8266-free-space : "); Serial.println(String(ESP.getFreeSketchSpace()));
  Serial.print("x-ESP8266-sketch-size : "); Serial.println(String(ESP.getSketchSize()));
  Serial.print("x-ESP8266-sketch-md5 : "); Serial.println(String(ESP.getSketchMD5()));
  Serial.print("x-ESP8266-chip-size : "); Serial.println( String(ESP.getFlashChipRealSize()));
  Serial.print("x-ESP8266-sdk-version : ");Serial.println( ESP.getSdkVersion());

}

void update_started() {
  USE_SERIAL.println("CALLBACK:  HTTP update process started");
}

void update_finished() {
  USE_SERIAL.println("CALLBACK:  HTTP update process finished");
}

void update_progress(int cur, int total) {
  USE_SERIAL.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
}

void update_error(int err) {
  USE_SERIAL.printf("CALLBACK:  HTTP update fatal error code %d\n", err);
}


void loop() {
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    WiFiClient client;

    // The line below is optional. It can be used to blink the LED on the board during flashing
    // The LED will be on during download of one buffer of data from the network. The LED will
    // be off during writing that buffer to flash
    // On a good connection the LED should flash regularly. On a bad connection the LED will be
    // on much longer than it will be off. Other pins than LED_BUILTIN may be used. The second
    // value is used to put the LED on. If the LED is on with HIGH, that value should be passed
    ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);

    // Add optional callback notifiers
    ESPhttpUpdate.onStart(update_started);
    ESPhttpUpdate.onEnd(update_finished);
    ESPhttpUpdate.onProgress(update_progress);
    ESPhttpUpdate.onError(update_error);

    t_httpUpdate_return ret = ESPhttpUpdate.update(client, host_name , "2");

    switch (ret) {
      case HTTP_UPDATE_FAILED:
        USE_SERIAL.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;

      case HTTP_UPDATE_NO_UPDATES:
        USE_SERIAL.println("HTTP_UPDATE_NO_UPDATES");
        break;

      case HTTP_UPDATE_OK:
        USE_SERIAL.println("HTTP_UPDATE_OK");
        break;
    }
  }
}
