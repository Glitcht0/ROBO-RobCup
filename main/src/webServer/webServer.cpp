#include "webServer.h"
#include <Arduino.h> 
#include "src/config.h"

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