#include "roboRemo.h"


Movimento movimentoAtual = {0, 0, 0};

/* 📲 Função para interpretar as mensagens enviadas pelo RoboRemo


*/ 
Movimento processarComandoRoboRemo(String cmd) {
    Movimento mov = {0, 0, 0};

    if (cmd.startsWith("joy")) {
        int espaco1 = cmd.indexOf(' ');

        if (espaco1 != -1) {
            mov.x = cmd.substring(3, espaco1).toInt();
            mov.y = cmd.substring(espaco1 + 1).toInt();
        }
    }
    else if (cmd == "F") {mov.y = 200;}
    else if (cmd == "B") {mov.y = -200;}
    else if (cmd == "L") {mov.x = 200;}
    else if (cmd == "R") {mov.x = -200;}
    else if (cmd == "FL") {mov.x = 200;mov.y = 200;}
    else if (cmd == "FR") {mov.x = -200;mov.y = 200;}
    else if (cmd == "BL") {mov.x = 200;mov.y = -200;}
    else if (cmd == "BR") {mov.x = -200;mov.y = -200;}
    else if (cmd == "360") {mov.z = 200;}

    return mov;
}



void RoboRemoLoop(){
    // 1. Verificar se há um novo cliente (RoboRemo) se conectando
    if (!roboRemoClient || !roboRemoClient.connected()) {
        roboRemoClient = roboRemoServer.available();
    }

    // 2. Ler comandos enviados pelo RoboRemo
    if (roboRemoClient && roboRemoClient.connected()) {
        while (roboRemoClient.available()) {
            String comando = roboRemoClient.readStringUntil('\n');
            comando.trim(); // Remove espaços 

            if (comando.length() > 0) {
                movimentoAtual = processarComandoRoboRemo(comando);
                ultimoPacoteTempo = millis(); // Reseta o timeout 
            }
        }
    }

    // 3. MÁQUINA DE ESTADOS E SEGURANÇA
    if (millis() - ultimoPacoteTempo > TIMEOUT_CONEXAO) {
        // ---- ESTADO 1: DESCONECTADO / SINAL PERDIDO ----
        movimentoAtual = {0, 0, 0}; // Zera a memória por segurança
        pararMotores();
        led("#ff9900"); // LARANJA: Aguardando conexão

    } else {
        //---- CONECTADO E OPERANDO ----
        led("#00ff00");

        // Aqui nós não chamamos mais a função. Nós apenas usamos 
        // os números que já estão guardados na nossa "memória".
        int velFrontL = movimentoAtual.y + movimentoAtual.x + movimentoAtual.z;
        int velBackL = movimentoAtual.y - movimentoAtual.x + movimentoAtual.z;
        int velFrontR = movimentoAtual.y - movimentoAtual.x - movimentoAtual.z;
        int velBackR = movimentoAtual.y + movimentoAtual.x - movimentoAtual.z;

        // Aplica aos motores
        aplicarVelocidadeMotores(velFrontL, velBackL, velFrontR, velBackR);
    }
}