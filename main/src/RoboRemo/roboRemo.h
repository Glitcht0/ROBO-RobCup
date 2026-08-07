#ifndef ROBO_REMO_H
#define ROBO_REMO_H
#include <Arduino.h>
#include <WiFi.h>
#include "src/MotorControll/Motor.h"



// Função para interpretar as mensagens enviadas pelo RoboRemo
struct Movimento {
    int x;
    int y;
    int z;
};

// Variáveis externas criadas no main.ino
extern WiFiServer roboRemoServer;
extern WiFiClient roboRemoClient;
extern unsigned long ultimoPacoteTempo;
extern const unsigned long TIMEOUT_CONEXAO;

Movimento processarComandoRoboRemo(String cmd);
extern Movimento movimentoAtual;
void RoboRemoLoop();

#endif