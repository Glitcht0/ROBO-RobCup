#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <WebServer.h>

// Coloque as credenciais da sua rede Wi-Fi aqui
// [http://192.168.48.110/](http://192.168.48.110/)
const char* ssid = "Marcos";
const char* password = "lucassilva2";

WebServer server(80);
String terminalBuffer = "";

void printWeb(String mensagem) {
  // Adiciona a nova mensagem com uma quebra de linha em HTML
  terminalBuffer += mensagem + "<br>";
  
  // Trava de segurança: Se o texto ficar gigante (mais de 3000 caracteres), 
  // ele limpa o começo para não estourar a memória RAM do robô
  if (terminalBuffer.length() > 3000) {
    terminalBuffer = terminalBuffer.substring(terminalBuffer.length() - 2000);
  }
}

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

  // 2. Configurações de Segurança do OTA
  // Define a porta padrão do ESP32 (3232)
  ArduinoOTA.setPort(3232);

  // Define o nome que vai aparecer no seu Arduino IDE (ex: GlitBot)
  ArduinoOTA.setHostname("GlitBot-proto-1");

  // DEFINE UMA SENHA PARA NINGUÉM HACKEAR O SEU ROBÔ NA ARENA!
  ArduinoOTA.setPassword("mvSSgYA27hLJunJmGerYW");

  // 3. Eventos do OTA (O que ele faz quando recebe o código)
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }
    Serial.println("Iniciando atualização OTA: " + type);
  });
  
  ArduinoOTA.onEnd([]() {
    Serial.println("\nAtualização Concluída!");
  });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progresso: %u%%\r", (progress / (total / 100)));
  });
  
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Erro fatal[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Falha de Autenticação");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Falha ao Iniciar");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Falha de Conexão");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Falha ao Receber");
    else if (error == OTA_END_ERROR) Serial.println("Falha na Finalização");
  });

  // 4. Inicia o serviço OTA
  ArduinoOTA.begin();

  // Rota 1: A Interface Visual (Página principal)
  server.on("/", []() {
    String html = "<html><body style='background:#121212; color:#00FF00; font-family:monospace; margin:20px;'>";
    html += "<h2>Terminal do GlitBot</h2><hr>";
    html += "<div id='log'></div>"; // Aqui é onde o texto vai aparecer
    
    // O JavaScript que faz a mágica de atualizar sem piscar
    html += "<script>";
    html += "setInterval(function() {";
    html += "  fetch('/log').then(response => response.text()).then(texto => {";
    html += "    document.getElementById('log').innerHTML = texto;";
    html += "    window.scrollTo(0, document.body.scrollHeight);"; // Faz a tela rolar para baixo sozinha!
    html += "  });";
    html += "}, 500);"; // Atualiza a cada 500 milissegundos
    html += "</script>";
    
    html += "</body></html>";
    
    server.send(200, "text/html", html);
  });

  // Rota 2: O Motor de Dados (Invisível para o usuário)
  server.on("/log", []() {
    server.send(200, "text/html", terminalBuffer);
  });
  
  server.begin();
  
  Serial.println("Pronto!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());

  
}
// 192.168.48.110
void loop() {
  // ESSA LINHA É OBRIGATÓRIA! Ela que fica escutando a rede.
  ArduinoOTA.handle();
  server.handleClient();
  printWeb("AOAOAOOA");
  delay(500);

  // Daqui para baixo, você coloca o código normal dos motores, PID, etc...
}