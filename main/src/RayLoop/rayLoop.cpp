#include "rayLoop.h"


// --- SUA FUNÇÃO DO PC (O CHEFE) ---
void RayLoop() {
  int tamanhoPacote = udp.parsePacket();
  if (tamanhoPacote == sizeof(PacoteDados)) {
    udp.read((char*)&pacote, sizeof(PacoteDados));
    ultimoPacoteTempo = millis(); // Reseta o timeout de segurança!
    
    // ATENÇÃO: Só liga os motores aqui dentro se o botão estiver ON
    if (pacote.powerAtivo) {
       led("#00ff00"); // VERDE: PC Ativo
       int velFrontL = (int)pacote.sliders[0];
       int velFrontR = (int)pacote.sliders[1];
       int velBackL  = (int)pacote.sliders[2];
       int velBackR  = (int)pacote.sliders[3];
       aplicarVelocidadeMotores(velFrontL, velBackL, velFrontR, velBackR);
    } else {
       pararMotores();
       led("#aa00ff"); // ROXO: PC Standby
    }
  } else if (tamanhoPacote > 0) {
    udp.flush(); // Limpa lixo da rede
  }
}

