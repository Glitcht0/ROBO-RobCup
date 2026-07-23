
#include "Engine/Assets/FontManager.h"
#include "Projects/RoboCup/States/Calibration/StateCalibration.h"
#include "Projects/RoboCup/States/Menu/StateMenu.h"
#include "Engine/Core/Engine.h"
#include "config.h"
#include "raylib.h"
#define RLIGHTS_IMPLEMENTATION
#include "libs/rlights.h"


#define RAYGUI_IMPLEMENTATION
#include "libs/raygui.h"
#include "Engine/resources/style/style_amber.h"
#include "Engine/Assets/FontManager.h"

#include "Engine/i18n/i18n.h"

/** ======================================================
    ⚽ Função Principal do Programa da RoboCup
    Aqui são setadas as configurações inciais do programa e 
    é iniciada a execução do loop principal pela Engine.

====================================================== */ 
int main(){
    
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE); 

    I18N_Load_File("Projects/RoboCup/Assets/lang/pt.json"); // Arquivo de textos
    
    InitWindow(LARGURA_TELA, ALTURA_TELA, "Raylib 3D - RoboCup");
    SetExitKey(KEY_NULL);

    SetTargetFPS(60);
    GuiLoadStyleAmber();

    Font uiFont = LoadFont();
    SetTextureFilter(uiFont.texture, TEXTURE_FILTER_BILINEAR); // Suaviza a fonte
    GuiSetFont(uiFont);

    

    GetStateMachine().ChangeState(new StateMenu()); // Muda para o estado de menu inicial do jogo

    EngineRun(); // Inicia o loop principal da Engine
    return 0;
}

       


