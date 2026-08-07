#pragma once


#include "libs/asio.hpp"
#include <raylib.h>
#include "libs/raygui.h"
#include "Engine/i18n/i18n.h"
#include "Engine/Core/State.h"
#include <string> 
#include <cstdint>

#include "Projects/RoboCup/src/Utils/PacoteDados.h"




#pragma once

#include "libs/asio.hpp"
#include <raylib.h>
#include "libs/raygui.h"
#include "Engine/i18n/i18n.h"
#include "Engine/Core/State.h"
#include <string> 
#include <cstdint>

#include "Projects/RoboCup/src/Utils/PacoteDados.h"

class StateMenu : public State {
public:
    StateMenu();
    ~StateMenu();
    void onEnter() override;
    void onExit() override;

    void update() override;
    void draw() override;
    
    Font uiFont;

private:
    void DrawButtonsMenu(float larguraTela, float alturaTela);
    void EnviarDadosControle(); // <-- Novo método para processar e enviar UDP

    // --- VARIÁVEIS DO ASIO ---
    asio::io_context m_io_context;         
    asio::ip::udp::socket m_socket;        
    std::vector<asio::ip::udp::endpoint> m_endpoints;  
    
    PacoteDados pacote;
};