#include "config.h"

String terminalBuffer = "";

const char* ssid = "Marcos";
const char* password = "lucassilva2";



void led(const char* hex) {
    uint32_t cor = strtoul(hex + 1, nullptr, 16); // Pula o '#'

    uint8_t r = (cor >> 16) & 0xFF;
    uint8_t g = (cor >> 8) & 0xFF;
    uint8_t b = cor & 0xFF;

    neopixelWrite(48,
                  r * BRILHO / 255,
                  g * BRILHO / 255,
                  b * BRILHO / 255);
}


void desligaTudo() {
    digitalWrite(MOTOR_LFw, LOW);
    digitalWrite(MOTOR_LFs, LOW);
    digitalWrite(MOTOR_LBw, LOW);
    digitalWrite(MOTOR_LBs, LOW);
    digitalWrite(MOTOR_RFw, LOW);
    digitalWrite(MOTOR_RFs, LOW);
    digitalWrite(MOTOR_RBw, LOW);
    digitalWrite(MOTOR_RBs, LOW);
}