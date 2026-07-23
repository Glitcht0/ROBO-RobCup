#pragma once


#include "libs/asio.hpp"
#include <raylib.h>
#include "libs/raygui.h"




#include "Engine/i18n/i18n.h"
#include "Engine/Core/State.h"
#include <string> 
#include <cstdint>

#pragma pack(push, 1)
struct PacoteDados {
    uint32_t id_mensagem;    
    float sliders[4];        
    bool powerAtivo;         
};
#pragma pack(pop)

class StateCalibration : public State {
public:
    StateCalibration();
    ~StateCalibration();
    void onEnter() override;
    void onExit() override;

    void update() override;
    void draw() override;
    
    Font uiFont;

private:
    float m_sliderValues[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    std::string m_sliderNomes[4] = { "FrontL", "FrontR", "BackL", "BackR"};
    // Variáveis para as caixinhas de correção (0 a 100%)
    int m_multiplicadores[4] = {100, 100, 100, 100}; 
    bool m_caixaEditando[4] = {false, false, false, false};
    
    bool m_checkValues[4] = { false, false, false, false };
    bool m_powerAtivo = false;

    // --- VARIÁVEIS DO ASIO ---
    asio::io_context m_io_context;         // O "motor" principal do Asio
    asio::ip::udp::socket m_socket;        // O nosso socket UDP
    asio::ip::udp::endpoint m_endpoint;    // O destino (Endereço e Porta)
    
    PacoteDados pacote;
};