#pragma once
#include "src/config.h"
#include <Arduino.h>
const uint8_t PWM_TESTE = 100; // 0-255 (~24%)

void motorloop(int velLF, int velLB, int velRF, int velRB);
void motorFrente(uint8_t frente, uint8_t tras, uint8_t pwm);
void motorTras(uint8_t frente, uint8_t tras, uint8_t pwm);
void pararMotores();
void motor(uint8_t frente, uint8_t tras, int velocidade);
void frente();
void tras();
void esquerda();
void direita();
void girarhorario();
void giraranti();