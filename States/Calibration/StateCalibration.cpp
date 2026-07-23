#include "StateCalibration.h"
#include "Projects/RoboCup/States/Menu/StateMenu.h"
#include <iostream>

// No construtor, inicializamos o socket linkando ele ao io_context
StateCalibration::StateCalibration() : m_socket(m_io_context) {
}

StateCalibration::~StateCalibration(){
}

void StateCalibration::onEnter(){
    try {
        // 1. Abre o socket para IPv4
        m_socket.open(asio::ip::udp::v4());
        
        // 2. Habilita a permissão de broadcast
        m_socket.set_option(asio::socket_base::broadcast(true));
        
        // 3. Define para onde vamos gritar: (Broadcast em todos os IPs locais, porta 6871)
        m_endpoint = asio::ip::udp::endpoint(asio::ip::address_v4::broadcast(), 6871);

        pacote.id_mensagem = 0;
        std::cout << "Servidor Asio UDP Broadcast iniciado na porta 6871..." << std::endl;
        
    } catch (std::exception& e) {
        std::cerr << "Erro ao iniciar socket Asio: " << e.what() << std::endl;
    }
}

void StateCalibration::onExit(){
    // Fecha o socket de forma segura
    if (m_socket.is_open()) {
        m_socket.close();
    }
}

void StateCalibration::update(){
    if (!m_socket.is_open()) return;

    pacote.id_mensagem++;
    pacote.powerAtivo = m_powerAtivo;
    for (int i = 0; i < 4; i++) {
        if (m_checkValues[i] == true){
            // Transforma o 90 inteiro da caixinha em 0.90 float e multiplica pelo slider
            float valorCorrigido = m_sliderValues[i] * (m_multiplicadores[i] / 100.0f);
            pacote.sliders[i] = valorCorrigido;
        }else{
            pacote.sliders[i] = 0;
        }
    }

    try {
        // Envia o pacote convertido para buffer no endpoint configurado
        m_socket.send_to(asio::buffer(&pacote, sizeof(PacoteDados)), m_endpoint);
    } catch (std::exception& e) {
        std::cerr << "Erro ao enviar pacote: " << e.what() << std::endl;
    }
}



void StateCalibration::draw()
{
    BeginDrawing();
    ClearBackground(BLACK);

    float larguraTela = GetRenderWidth();
    float alturaTela = GetRenderHeight();

    float sx = larguraTela / 1280.0f;
    float sy = alturaTela / 720.0f;
    float s = (sx < sy) ? sx : sy;

    // --- SEUS BOTÕES CENTRAIS ORIGINAIS ---
    float btnW = 140 * s;
    float btnH = 30 * s;
    float xCentro = larguraTela/2 - btnW/2;
    float y0 = 300 * s;
    float dy = 40 * s;

    if (GuiButton({ xCentro, y0 + 1*dy, btnW, btnH }, "Menu2")) {
        ChangeState(new StateMenu());
    }
    if (GuiButton({ xCentro, y0 + 2*dy, btnW, btnH }, "Menu3")) {
        // ChangeState(new StateMenu());
    }

    // --- NOVA COLUNA ESQUERDA (CALIBRAÇÃO) ---
    float margemEsq = 20 * s;         // Distância da borda esquerda
    float startY = 50 * s;            // Altura inicial
    float checkSize = 25 * s;         // Tamanho do checkbox
    float sliderW = 500 * s;          // Largura do slider
    float sliderH = 25 * s;           // Altura do slider
    float espacamentoY = 45 * s;      // Distância vertical entre cada linha
    float espacamentoX = 65 * s;      // Distância entre o checkbox e o slider

    for (int i = 0; i < 4; i++){
        float linhaY = startY + (i * espacamentoY);
        
        // 1. Caixinha de Seleção
        GuiCheckBox({ margemEsq, linhaY, checkSize, checkSize }, "", &m_checkValues[i]);
        
        // 2. Slider (posicionado à direita do checkbox)
        float sliderX = margemEsq + checkSize + espacamentoX;
        GuiSlider({ sliderX, linhaY, sliderW, sliderH }, m_sliderNomes[i].c_str(), TextFormat("%.0f", m_sliderValues[i]), &m_sliderValues[i], -255.0f, 255.0f);

        // 3. Caixinha de Correção (ValueBox) à direita do slider
        float caixaX = sliderX + sliderW + (50 * s); // Dá um espaço de 20 pixels após o slider
        float caixaW = 55 * s; // Largura da caixinha de texto

        // Se clicar na caixinha, inverte o modo de edição (permite digitar)
        if (GuiValueBox({ caixaX, linhaY, caixaW, sliderH }, "%", &m_multiplicadores[i], 0, 100, m_caixaEditando[i])) {
            m_caixaEditando[i] = !m_caixaEditando[i];
        }
    }

    // Botão de Liga/Desliga (Toggle) abaixo da lista
    float toggleY = startY + (4 * espacamentoY) + 15 * s;
    // Ocupa a largura total da coluna (check + espaco + slider + espaco + caixinha)
    float toggleW = checkSize + espacamentoX + sliderW + (20 * s) + (55 * s); 
    
    // O GuiToggle altera o valor booleano quando clicado
    GuiToggle({ margemEsq, toggleY, toggleW, 35 * s }, m_powerAtivo ? "LIGADO" : "DESLIGADO", &m_powerAtivo);

    EndDrawing();

}