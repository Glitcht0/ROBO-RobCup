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
// --- Configurações de Conexão RoboRemo (TCP Server na porta 9876) ---
WiFiServer roboRemoServer(9876);
WiFiClient roboRemoClient;

// --- Variáveis de Controle Mecanum ---
int vX = 0; // Eixo X (Lateral)
int vY = 0; // Eixo Y (Frente/Trás)
int vZ = 0; // Rotação (Giro)

// --- Controle de Segurança (Timeout) ---
unsigned long ultimoPacoteTempo = 0;
const unsigned long TIMEOUT_CONEXAO = 500; // 500ms sem dados para segurança

WebServer server(80);

// [http://192.168.48.110/](http://192.168.48.110/)

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
  printWeb("Escutando pacotes da Calibração na porta " + String(portaUDP) + ".......");

 roboRemoServer.begin();
 Serial.println("Servidor RoboRemo TCP iniciado na porta 9876");

  
}





// 192.168.48.106
void loop() {
  ArduinoOTA.handle(); // Fica escutando a rede para atualizações
  server.handleClient(); // Servidor Web
  
  // 1. Verificar se há um novo cliente (RoboRemo) se conectando
 if (!roboRemoClient || !roboRemoClient.connected()) {
  roboRemoClient = roboRemoServer.available();
 }

 // 2. Ler comandos enviados pelo RoboRemo
 if (roboRemoClient && roboRemoClient.connected()) {
  while (roboRemoClient.available()) {
   String comando = roboRemoClient.readStringUntil('\n');
   comando.trim(); // Remove espaços ou quebras de linha indesejadas
  
   if (comando.length() > 0) {
    processarComandoRoboRemo(comando);
    ultimoPacoteTempo = millis(); // Reseta o timeout de segurança
   }
  }
 }

 // 3. MÁQUINA DE ESTADOS E SEGURANÇA
 if (millis() - ultimoPacoteTempo > TIMEOUT_CONEXAO) {
  // ---- ESTADO 1: DESCONECTADO / SINAL PERDIDO ----
  vX = 0; vY = 0; vZ = 0;
  pararMotores();
  led("#ff9900"); // LARANJA: Aguardando conexão
 } else {
  // ---- ESTADO 2: CONECTADO E OPERANDO ----
  led("#00ff00"); // VERDE: Ativo

  // Cinemática Inversa para Rodas Mecanum
  // vY: Frente/Trás, vX: Deslocamento lateral, vZ: Rotação
  int velFrontL = vY + vX + vZ;
  int velBackL = vY - vX + vZ;
  int velFrontR = vY - vX - vZ;
  int velBackR = vY + vX - vZ;

  // Aplica aos motores
  motorloop(velFrontL, velBackL, velFrontR, velBackR);
 }
}

// Função para interpretar as mensagens enviadas pelo RoboRemo
void processarComandoRoboRemo(String cmd) {
 // Exemplo de protocolo via Joystick do RoboRemo enviando "x[val] y[val]" ou comandos textuais
 // Se estiver usando Joystick do tipo "x y" (ex: "120 200"):
 if (cmd.startsWith("joy")) {
  // Exemplo: joystick envia "joy X Y"
  int espaco1 = cmd.indexOf(' ');
  if (espaco1 != -1) {
   vX = cmd.substring(3, espaco1).toInt();
   vY = cmd.substring(espaco1 + 1).toInt();
  }
 }
 else if (cmd == "stop" || cmd == "A0") {
  vX = 0; vY = 0; vZ = 0;
 }
 // Você também pode mapear botões simples, ex: "F" (Frente), "B" (Trás), etc.
 else if (cmd == "F") { vY = 200; vX = 0; vZ = 0; }
 else if (cmd == "B") { vY = -200; vX = 0; vZ = 0; }
 else if (cmd == "R") { vY = 0; vX = -200; vZ = 0; }
 else if (cmd == "L") { vY = 0; vX = 200; vZ = 0; }
 else if (cmd == "S") { vY = 0; vX = 0; vZ = 0; }
 else if (cmd == "FL") { vY = 200; vX = 200; vZ = 0; }
 else if (cmd == "BR") { vY = -200; vX = -200; vZ = 0; }
 else if (cmd == "BL") { vY = -200; vX = 200; vZ = 0; }
 else if (cmd == "FR") { vY = 200; vX = -200; vZ = 0; }
 else if (cmd == "360") { vY = 0; vX = 0; vZ = 200; }
}
