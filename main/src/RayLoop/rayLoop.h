#pragma once
#include "src/config.h"
#include <Arduino.h>
#include <WiFi.h>      // OBRIGATÓRIO: Define WiFiUDP
#include <WiFiUdp.h>
#include "src/MotorControll/Motor.h"

// Estrutura do pacote de dados do Raylib
struct __attribute__((packed)) PacoteDados {
  uint32_t id_mensagem;
  float sliders[4];
  bool powerAtivo;
};

// Variáveis externas criadas no main.ino
extern WiFiUDP udp;
extern PacoteDados pacote;
extern unsigned long ultimoPacoteTempo;

void RayLoop();


