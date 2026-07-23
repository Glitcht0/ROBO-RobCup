#pragma once


#include <raylib.h>
#include "libs/raygui.h"
#include "Engine/i18n/i18n.h"
#include "Engine/Core/State.h"





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
};