#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <WebServer.h>

#include "src/config.h"
#include "src/ota/Ota.h"
#include "src/webServer/webServer.h"


// [http://192.168.48.110/](http://192.168.48.110/)


WebServer server(80);


void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando Robô...");

  // 1. Conecta no Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Conexão falhou! Reiniciando...");
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
  
  server.begin();
  
  Serial.println("Pronto!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());

  
}

// 192.168.48.110
void loop() {
  ArduinoOTA.handle(); // fica escutando a rede.
  server.handleClient();
  printWeb("AOAOAOOA");
  delay(500);

  
}