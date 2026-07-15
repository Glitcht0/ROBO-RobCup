#include "Ota.h"
#include <ArduinoOTA.h>
#include "webServer.h"
#include "src/config.h"

void onOTAStart() {
    String type;

    if (ArduinoOTA.getCommand() == U_FLASH) {
        type = "sketch";
    } else {
        type = "filesystem";
    }

    Serial.println("Iniciando atualização OTA: " + type);
}

void onOTAError(ota_error_t error) {
    Serial.printf("Erro fatal[%u]: ", error);

    if (error == OTA_AUTH_ERROR)
        Serial.println("Falha de Autenticação");
    else if (error == OTA_BEGIN_ERROR)
        Serial.println("Falha ao Iniciar");
    else if (error == OTA_CONNECT_ERROR)
        Serial.println("Falha de Conexão");
    else if (error == OTA_RECEIVE_ERROR)
        Serial.println("Falha ao Receber");
    else if (error == OTA_END_ERROR)
        Serial.println("Falha na Finalização");
}





extern String terminalBuffer;

void printWeb(String mensagem) {
  terminalBuffer += mensagem + "<br>"; // Quebra de Linha
  
  //Mais de 3000 caracteres, limpa 
  if (terminalBuffer.length() > 3000) {
    terminalBuffer = terminalBuffer.substring(terminalBuffer.length() - 2000);
  }
}

const char HTML[] PROGMEM = R"rawliteral(
<html>
<body style="background:#121212; color:#00FF00; font-family:monospace; margin:20px;">
<h2>Terminal do GlitBot</h2>
<hr>
<div id="log"></div>

<script>
setInterval(function() {
    fetch('/log')
    .then(response => response.text())
    .then(texto => {
        document.getElementById('log').innerHTML = texto;
        window.scrollTo(0, document.body.scrollHeight);
    });
}, 500);
</script>

</body>
</html>
)rawliteral";

extern WebServer server;

void handleRoot() {
    server.send(200, "text/html", HTML);
}

void handleLog() {
    server.send(200, "text/plain", terminalBuffer);
}