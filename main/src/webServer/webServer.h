#pragma once
#include <Arduino.h>     // String e PROGMEM
#include <WebServer.h>   // WebServer

extern WebServer server;

void printWeb(String mensagem);
void handleRoot();

extern const char HTML[] PROGMEM;