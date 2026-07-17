#include "RoboCup.h"

StateCalibration::StateCalibration()
{
}

StateCalibration::~StateCalibration()
{
}

void StateCalibration::onEnter()
{
}

void StateCalibration::onExit()
{
}

void StateCalibration::update()
{
    
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

    float btnW = 140 * s;
    float btnH = 30 * s;
    float x = larguraTela/2 - btnW/2;

    float y0 = 300 * s;
    float dy = 40 * s;
    if (GuiButton({ x, y0 + 1*dy, btnW, btnH }, T("Menu2"))) {
        ChangeState(new StateMenu());
    }
    if (GuiButton({ x, y0 + 2*dy, btnW, btnH }, T("Menu3"))) {
        ChangeState(new StateMenu());
    }

    

    EndDrawing();
}