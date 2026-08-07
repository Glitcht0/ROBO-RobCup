#include "StateMenu.h"
#include "Projects/RoboCup/States/Calibration/StateCalibration.h"
#include <iostream>

StateMenu::StateMenu() : m_socket(m_io_context){}

StateMenu::~StateMenu(){}

void StateMenu::onEnter(){
    try {
        m_socket.open(asio::ip::udp::v4());
        m_socket.set_option(asio::socket_base::broadcast(true));
        
        m_endpoints.clear();
        m_endpoints.push_back(asio::ip::udp::endpoint(asio::ip::address_v4::broadcast(), 6871));
        m_endpoints.push_back(asio::ip::udp::endpoint(asio::ip::make_address("10.42.0.255"), 6871));
        m_endpoints.push_back(asio::ip::udp::endpoint(asio::ip::make_address("10.10.31.255"), 6871));

        pacote.id_mensagem = 0;
        std::cout << "Servidor Asio UDP Broadcast iniciado na porta 6871 (Multi-Interface)..." << std::endl;
        
    } catch (std::exception& e) {
        std::cerr << "Erro ao iniciar socket Asio: " << e.what() << std::endl;
    }
}

void StateMenu::onExit(){
    if (m_socket.is_open()) {
        m_socket.close();
    }
}

void StateMenu::update(){
    if (!m_socket.is_open()) return; // Só calcula se a rede estiver pronta

    pacote.id_mensagem++;

    if (IsGamepadAvailable(0)) {
        // Leitura dos analógicos (-1.0 a 1.0) multiplicada pela força (255)
        // O Eixo Y no Raylib é invertido (negativo é para cima), então invertemos o sinal para bater com o seu 'F -> y = 200'
        float eixoY = -GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) * 255.0f; 
        float eixoX = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X) * 255.0f;
        float eixoZ = GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_X) * 255.0f; // Rotação no analógico direito

        // Deadzone: Ignora ruídos do analógico genérico se não estivermos tocando nele
        if (abs(eixoY) < 30.0f) eixoY = 0.0f;
        if (abs(eixoX) < 30.0f) eixoX = 0.0f;
        if (abs(eixoZ) < 30.0f) eixoZ = 0.0f;

        // --- CÁLCULO CINEMÁTICO (ROBÔ OMNIDIRECIONAL) ---
        // Baseado na sua ordem de sliders: 0=FrontL, 1=FrontR, 2=BackL, 3=BackR
        pacote.sliders[0] = eixoY + eixoX + eixoZ; // FrontL
        pacote.sliders[1] = eixoY - eixoX - eixoZ; // FrontR
        pacote.sliders[2] = eixoY - eixoX + eixoZ; // BackL
        pacote.sliders[3] = eixoY + eixoX - eixoZ; // BackR

        // --- NORMALIZAÇÃO E AJUSTE DE INÉRCIA ---
        float pwmMinimo = 100.0f; // Força mínima para a roda sair do lugar sem travar
        float pwmMaximo = 255.0f; // Velocidade máxima (não exceder 255)

        for (int i = 0; i < 4; i++) {
            float forcaAbsoluta = abs(pacote.sliders[i]); // Pega a força ignorando se é frente ou trás

            if (forcaAbsoluta > 0.1f) {
                // Mapeia o movimento do controle (0 a 255) para a janela útil do motor (100 a 255)
                // Isso cria uma aceleração suave sem o ponto cego onde o motor trava
                float forcaMapeada = pwmMinimo + (forcaAbsoluta / 255.0f) * (pwmMaximo - pwmMinimo);

                if (forcaMapeada > pwmMaximo) forcaMapeada = pwmMaximo; // Trava o teto

                // Devolve o sinal correto para o motor (frente ou trás)
                if (pacote.sliders[i] < 0.0f) {
                    pacote.sliders[i] = -forcaMapeada;
                } else {
                    pacote.sliders[i] = forcaMapeada;
                }
            } else {
                pacote.sliders[i] = 0.0f; // Parado total (caiu na deadzone)
            }
        }

        // --- SISTEMA DE IGNIÇÃO (TRAVA DE SEGURANÇA) ---
        // Pressionar o botão "Start" ou "Option" liga o motor
        if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT)) {
            pacote.powerAtivo = true;
        }
        // Pressionar o "Select" ou "Share" desliga o motor (Emergência)
        if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_LEFT)) {
            pacote.powerAtivo = false;
        }

    } else { // Se o if (IsGamepadAvailable(0)) for falso (controle desconectado)
        // Medida de segurança: Zera os motores e corta a ignição
        for (int i = 0; i < 4; i++) {
            pacote.sliders[i] = 0.0f;
        }
        pacote.powerAtivo = false;
    }

    // Dispara o pacote UDP
    EnviarDadosControle();
}

void StateMenu::EnviarDadosControle()
{
    try {
        // Envia o pacote convertido para buffer em todas as rotas de rede configuradas
        for (const auto& endpoint : m_endpoints) {
            m_socket.send_to(asio::buffer(&pacote, sizeof(PacoteDados)), endpoint);
        }
    } catch (std::exception& e) {
        std::cerr << "Erro ao enviar pacote UDP no Menu: " << e.what() << std::endl;
    }
}

void StateMenu::draw()
{
    BeginDrawing();
    ClearBackground(BLACK);

    float larguraTela = GetRenderWidth();
    float alturaTela = GetRenderHeight();

    float sx = larguraTela / 1280.0f;
    float sy = alturaTela / 720.0f;
    float s = (sx < sy) ? sx : sy;

    float btnW = 140 * s;
    float btnH = 30 * s;
    float x = larguraTela/2 - btnW/2;

    float y0 = 300 * s;
    float dy = 40 * s;
    if (GuiButton({ x, y0 + 1*dy, btnW, btnH }, T("Menu4"))) {
        ChangeState(new StateCalibration());
    }

    // --- LEITURA E DESENHO DO CONTROLE MOVIDOS PARA O LOOP DE RENDERIZAÇÃO ---
    if (IsGamepadAvailable(0)) {
        
        // PAINEL ESQUERDO: Instruções e Matemática
        DrawText("INSTRUÇÕES DO ROBÔ:", 50, 50, 20, GREEN);
        DrawText("- Analógico Esquerdo Cima/Baixo: Frente (F) / Trás (B)", 50, 80, 20, WHITE);
        DrawText("- Analógico Esquerdo Esq/Dir: Strafe Left (L) / Right (R)", 50, 110, 20, WHITE);
        DrawText("- Analógico Direito Esq/Dir: Rotação 360", 50, 140, 20, WHITE);
        
        DrawText("MOTORES (ENVIADOS VIA UDP):", 50, 190, 20, GREEN);
        DrawText(TextFormat("Front Left: %.0f", pacote.sliders[0]), 50, 220, 25, WHITE);
        DrawText(TextFormat("Front Right: %.0f", pacote.sliders[1]), 50, 250, 25, WHITE);
        DrawText(TextFormat("Back Left: %.0f", pacote.sliders[2]), 50, 280, 25, WHITE);
        DrawText(TextFormat("Back Right: %.0f", pacote.sliders[3]), 50, 310, 25, WHITE);
        
        DrawText(TextFormat("Status Power: %s", pacote.powerAtivo ? "LIGADO" : "DESLIGADO"), 50, 350, 20, pacote.powerAtivo ? GREEN : RED);


        // PAINEL DIREITO: Leitura RAW que você pediu para manter
        DrawText("LEITURA RAW DO HARDWARE:", 500, 50, 20, YELLOW);
        DrawText(TextFormat("Nome: %s", GetGamepadName(0)), 500, 80, 20, YELLOW);
        
        DrawText(TextFormat("Eixo Físico 0 (X Esq): %.2f", GetGamepadAxisMovement(0, 0)), 500, 110, 20, YELLOW);
        DrawText(TextFormat("Eixo Físico 1 (Y Esq): %.2f", GetGamepadAxisMovement(0, 1)), 500, 140, 20, YELLOW);
        DrawText(TextFormat("Eixo Físico 2 (Gatilho/Z): %.2f", GetGamepadAxisMovement(0, 2)), 500, 170, 20, YELLOW);
        DrawText(TextFormat("Eixo Físico 3 (X Dir): %.2f", GetGamepadAxisMovement(0, 3)), 500, 200, 20, YELLOW);
        DrawText(TextFormat("Eixo Físico 4 (Y Dir): %.2f", GetGamepadAxisMovement(0, 4)), 500, 230, 20, YELLOW);

        int rawBtn = GetGamepadButtonPressed();
        if (rawBtn > 0) {
            DrawText(TextFormat("ÚLTIMO BOTÃO RAW APERTADO: %d", rawBtn), 500, 280, 20, YELLOW);
        }

    } else {
        DrawText("Nenhum controle pareado. Robô parado.", 50, 50, 30, RED);
    }

    EndDrawing();
}