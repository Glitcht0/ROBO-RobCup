#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>


#include "src/config.h"
#include "src/ota/Ota.h"
#include "src/webServer/webServer.h"
#include "src/MotorControll/Motor.h"


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

  desligaTudo();

  led("#00ff00");



  
}





// 192.168.48.110
void loop() {
  ArduinoOTA.handle(); // fica escutando a rede.
  server.handleClient();
  
  
  /*if (millis() - ultimoTempo >= 500) {
    ultimoTempo = millis();
    printWeb("AOAOAOOA");
    Serial.println("AOAOAOOA");
  }*/

  /*if (millis() - ultimoTeste >= 2000) {
      ultimoTeste = millis();

      desligaTudo();

      switch (etapa) {
          case 0: led("#ff0000"); digitalWrite(MOTOR_LFw, HIGH); break;
          case 1: led("#880000"); digitalWrite(MOTOR_LFs, HIGH); break;
          case 2: led("#00ff00"); digitalWrite(MOTOR_LBw, HIGH); break;
          case 3: led("#008800"); digitalWrite(MOTOR_LBs, HIGH); break;
          case 4: led("#0000ff"); digitalWrite(MOTOR_RFw, HIGH); break;
          case 5: led("#000088"); digitalWrite(MOTOR_RFs, HIGH); break;
          case 6: led("#ffff00"); digitalWrite(MOTOR_RBw, HIGH); break;
          case 7: led("#888800"); digitalWrite(MOTOR_RBs, HIGH); break;
          default:
              etapa = 0;
              return;
      }

      etapa++;
  }*/

  motorloop();



  
}