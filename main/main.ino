#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>


#include "src/config.h"
#include "src/ota/Ota.h"
#include "src/webServer/webServer.h"
#include "src/MotorControll/Motor.h"

#include "src/RayLoop/RayLoop.h"
#include "src/roboRemo/roboRemo.h"
#include <WiFiUdp.h>

// --- Configurações de Rede UDP ---
WiFiUDP udp;
const unsigned int portaUDP = 6871;


// --- Configurações de Conexão RoboRemo (TCP Server na porta 9876) ---
WiFiServer roboRemoServer(9876);
WiFiClient roboRemoClient;


// --- Controle de Segurança (Timeout) ---
unsigned long ultimoPacoteTempo = 0;
const unsigned long TIMEOUT_CONEXAO = 500; // 500ms sem dados para segurança

WebServer server(80);



unsigned long ultimoTempo = 0;
uint8_t etapa = 0;
unsigned long ultimoTeste = 0;

PacoteDados pacote;


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
 printWeb("Servidor RoboRemo TCP iniciado na porta 9876");

  
}





// --- O SEU NOVO LOOP PRINCIPAL ---
void loop() {
  ArduinoOTA.handle(); 
  server.handleClient(); 
  
  
  RayLoop(); // 1. Sempre tenta ler o PC primeiro

  // 2. Sistema de Hierarquia
  if (millis() - ultimoPacoteTempo <= TIMEOUT_CONEXAO) {
    // O PC ESTÁ VIVO E CONECTADO!
    // Esvaziamos o buffer do TCP para o celular não acumular lixo na memória
    if (roboRemoClient && roboRemoClient.available()) {
        roboRemoClient.readString(); 
    }
  } 
  else {
    RoboRemoLoop();
  }
}
