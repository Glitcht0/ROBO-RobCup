#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>


#include "src/config.h"
#include "src/ota/Ota.h"
#include "src/webServer/webServer.h"
#include "src/MotorControll/Motor.h"
#include <WiFiUdp.h>

// --- Configurações de Rede UDP ---
WiFiUDP udp;
const unsigned int portaUDP = 6871;

// --- Estrutura de Dados (Exatamente igual ao PC) ---
// O __attribute__((packed)) garante que o ESP32 não adicione bytes vazios na memória,
// mantendo os exatos 21 bytes que o seu Windows envia.
struct __attribute__((packed)) PacoteDados {
  uint32_t id_mensagem;
  float sliders[4];
  bool powerAtivo;
};

PacoteDados pacote;

// --- Controle de Segurança (Timeout) ---
unsigned long ultimoPacoteTempo = 0;
const unsigned long TIMEOUT_CONEXAO = 500; // Tempo máximo sem receber dados (500ms)

// [http://192.168.48.110/](http://192.168.48.110/)


WebServer server(80);
unsigned long ultimoTempo = 0;
uint8_t etapa = 0;
unsigned long ultimoTeste = 0;




void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando Robô...");
  led("#103af8");

  // 1. Conecta no Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Conexão falhou! Reiniciando...");
    led("#fc1212");
    
    delay(5000);
    ESP.restart();
  }

  
  ArduinoOTA.setPort(3232); // Configurações de Segurança do OTA, Define a porta padrão do ESP32 (3232)
  ArduinoOTA.setHostname("GlitBot-proto-1"); // Define o nome que vai aparecer no seu Arduino IDE (ex: GlitBot)
  ArduinoOTA.setPassword("mvSSgYA27hLJunJmGerYW"); // DEFINE UMA SENHA PARA NINGUÉM HACKEAR O SEU ROBÔ NA ARENA!

  // ========  Eventos do OTA (O que ele faz quando recebe o código) ========
  ArduinoOTA.onStart(onOTAStart);
  
  ArduinoOTA.onEnd([]() {Serial.println("\nAtualização Concluída!");});
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {Serial.printf("Progresso: %u%%\r", (progress / (total / 100)));});
  ArduinoOTA.onError(onOTAError);

  ArduinoOTA.begin();

 

  
  server.on("/", handleRoot);
  server.on("/log", handleLog);
  
  server.begin();
  
  Serial.println("Pronto!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());

  // =========== Configuração dos pinos do motor ==========================
  pinMode(MOTOR_LFw, OUTPUT);
  pinMode(MOTOR_LFs, OUTPUT);

  pinMode(MOTOR_LBw, OUTPUT);
  pinMode(MOTOR_LBs, OUTPUT);

  pinMode(MOTOR_RFw, OUTPUT);
  pinMode(MOTOR_RFs, OUTPUT);

  pinMode(MOTOR_RBw, OUTPUT);
  pinMode(MOTOR_RBs, OUTPUT);

  pararMotores();

  led("#aa00ff"); // ROXO: Conectado e em Standby

  // Inicia o UDP na porta definida
  udp.begin(portaUDP);
  Serial.printf("Escutando pacotes da Calibração na porta %d...\n", portaUDP);
  // Usando a concatenação de Strings do Arduino:
  printWeb("Escutando pacotes da Calibração na porta " + String(portaUDP) + "...");



  
}





// 192.168.48.106
void loop() {
  ArduinoOTA.handle(); // Fica escutando a rede para atualizações
  server.handleClient(); // Servidor Web
  
  // 1. LER PACOTES UDP
  int tamanhoPacote = udp.parsePacket();
  if (tamanhoPacote == sizeof(PacoteDados)) {
    // Se o pacote tem o tamanho exato de 21 bytes, lemos para a struct
    udp.read((char*)&pacote, sizeof(PacoteDados));
    ultimoPacoteTempo = millis(); // Reseta o cronômetro de segurança!
  } else if (tamanhoPacote > 0) {
    // Se chegou lixo ou um pacote de tamanho errado, esvazia o buffer
    udp.flush(); 
  }

  // 2. MÁQUINA DE ESTADOS E CORES (Segurança e Operação)
  if (millis() - ultimoPacoteTempo > TIMEOUT_CONEXAO) {
    // ---- ESTADO 1: DESCONECTADO / SINAL PERDIDO ----
    // PC travou, Raylib fechou ou Wi-Fi falhou.
    pararMotores(); // CORRIGIDO: Usa a função certa para zerar o PWM
    led("#ff9900"); // LARANJA: Aguardando conexão do PC

  } else {
    // ---- ESTADO 2: CONECTADO ----
    if (pacote.powerAtivo == false) {
      // STANDBY: O Raylib está enviando pacotes, mas o botão "LIGADO/DESLIGADO" está desligado
      pararMotores(); // CORRIGIDO: Usa a função certa para zerar o PWM
      led("#aa00ff"); // ROXO: Conectado e em Standby (Pronto para a ação)
      
    } else {
      // OPERAÇÃO: Conectado e o botão do Raylib está LIGADO
      led("#00ff00"); // VERDE: Ativo! Motores operando.

      // --- LÓGICA DOS MOTORES ---
      // Nomeando as variáveis de acordo com os seus sliders do PC para evitar confusão
      int velFrontL = (int)pacote.sliders[0];
      int velFrontR = (int)pacote.sliders[1];
      int velBackL  = (int)pacote.sliders[2];
      int velBackR  = (int)pacote.sliders[3];

      // CORRIGIDO: Passando os valores na ordem exata que a motorloop() pede:
      // (Frente-Esq, Trás-Esq, Frente-Dir, Trás-Dir)
      motorloop(velFrontL, velBackL, velFrontR, velBackR);
    }
  }
}