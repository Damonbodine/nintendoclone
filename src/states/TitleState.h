#pragma once

#include "states/GameState.h"

class Game;

class TitleState : public GameState {
public:
    TitleState(Game& game);

    void enter() override;
    void exit() override;
    void update(Game& game) override;
    void render(SDL_Renderer* renderer) override;

private:
    Game& m_game;
    int m_blinkTimer = 0;
    bool m_showPressStart = true;
};
