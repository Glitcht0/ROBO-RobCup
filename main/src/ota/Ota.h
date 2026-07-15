#pragma once
#include <ArduinoOTA.h>
#include <Arduino.h>     // String e PROGMEM
#include <WebServer.h>   // WebServer

extern WebServer server;

void printWeb(String mensagem);
void handleRoot();
void handleLog();

extern const char HTML[] PROGMEM;
void onOTAStart();
void onOTAError(ota_error_t error);

