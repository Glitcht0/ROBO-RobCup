#include "Motor.h"


void motorFrente(uint8_t frente, uint8_t tras, uint8_t pwm) {
    analogWrite(frente, pwm);
    analogWrite(tras, 0);
}

void motorTras(uint8_t frente, uint8_t tras, uint8_t pwm) {
    analogWrite(frente, 0);
    analogWrite(tras, pwm);
}

void pararMotores() {
    analogWrite(MOTOR_LFw, 0);
    analogWrite(MOTOR_LFs, 0);

    analogWrite(MOTOR_LBw, 0);
    analogWrite(MOTOR_LBs, 0);

    analogWrite(MOTOR_RFw, 0);
    analogWrite(MOTOR_RFs, 0);

    analogWrite(MOTOR_RBw, 0);
    analogWrite(MOTOR_RBs, 0);
}

void motor(uint8_t frente, uint8_t tras, int velocidade) {
    velocidade = constrain(velocidade, -255, 255);

    if (velocidade > 0) {
        analogWrite(frente, velocidade);
        analogWrite(tras, 0);
    }
    else if (velocidade < 0) {
        analogWrite(frente, 0);
        analogWrite(tras, -velocidade);
    }
    else {
        analogWrite(frente, 0);
        analogWrite(tras, 0);
    }
}

void frente() {
  motor(MOTOR_LFw, MOTOR_LFs, PWM_TESTE);
  motor(MOTOR_LBw, MOTOR_LBs, PWM_TESTE);
  motor(MOTOR_RFw, MOTOR_RFs, PWM_TESTE);
  motor(MOTOR_RBw, MOTOR_RBs, PWM_TESTE);

    led("#00ff00");
}

void tras() {
  motor(MOTOR_LFw, MOTOR_LFs, -PWM_TESTE);
  motor(MOTOR_LBw, MOTOR_LBs, -PWM_TESTE);
  motor(MOTOR_RFw, MOTOR_RFs, -PWM_TESTE);
  motor(MOTOR_RBw, MOTOR_RBs, -PWM_TESTE);

    led("#ff0000");
}

void esquerda() {
  motor(MOTOR_LFw, MOTOR_LFs, -PWM_TESTE);
  motor(MOTOR_LBw, MOTOR_LBs,  PWM_TESTE);
  motor(MOTOR_RFw, MOTOR_RFs,  PWM_TESTE);
  motor(MOTOR_RBw, MOTOR_RBs, -PWM_TESTE);

    led("#0000ff");
}


void direita() {
  motor(MOTOR_LFw, MOTOR_LFs,  PWM_TESTE);
  motor(MOTOR_LBw, MOTOR_LBs, -PWM_TESTE);
  motor(MOTOR_RFw, MOTOR_RFs, -PWM_TESTE);
  motor(MOTOR_RBw, MOTOR_RBs,  PWM_TESTE);

    led("#ffff00");
}

void girarhorario() {
  motor(MOTOR_LFw, MOTOR_LFs,  PWM_TESTE);
  motor(MOTOR_LBw, MOTOR_LBs,  PWM_TESTE);
  motor(MOTOR_RFw, MOTOR_RFs, -PWM_TESTE);
  motor(MOTOR_RBw, MOTOR_RBs, -PWM_TESTE);

    led("#ff00ff");
}

void giraranti() {
  motor(MOTOR_LFw, MOTOR_LFs, -PWM_TESTE);
  motor(MOTOR_LBw, MOTOR_LBs, -PWM_TESTE);
  motor(MOTOR_RFw, MOTOR_RFs,  PWM_TESTE);
  motor(MOTOR_RBw, MOTOR_RBs,  PWM_TESTE);

    led("#00ffff");
}

void motorloop(int velLF, int velLB, int velRF, int velRB) {
    // A sua função motor() já faz a mágica de separar positivo (frente) e negativo (trás)
    motor(MOTOR_LFw, MOTOR_LFs, velLF);
    motor(MOTOR_LBw, MOTOR_LBs, velLB);
    motor(MOTOR_RFw, MOTOR_RFs, velRF);
    motor(MOTOR_RBw, MOTOR_RBs, velRB);
}