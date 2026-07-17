#ifndef CONFIG_H
#define CONFIG_H
#include <cstdint>
#include <Arduino.h> 

#define BRILHO 8

// Definições de pinos para o controle do motor
const uint8_t MOTOR_LFw = 6;
const uint8_t MOTOR_LFs = 7;

const uint8_t MOTOR_LBw = 5;
const uint8_t MOTOR_LBs = 4;

const uint8_t MOTOR_RFw = 15; 
const uint8_t MOTOR_RFs = 16;

const uint8_t MOTOR_RBw = 18;
const uint8_t MOTOR_RBs = 17;

// Credenciais para o Wifi
extern const char* ssid;
extern const char* password;

extern String terminalBuffer;
void led(const char* hex);
void desligaTudo();

#endif