#include "Ota.h"
#include <ArduinoOTA.h>

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