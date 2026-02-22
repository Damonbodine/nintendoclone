#pragma once

#include "states/GameState.h"
#include "graphics/SpriteSheet.h"
#include <string>

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

    // Assets
    SpriteSheet m_fontSheet;
    SpriteSheet m_tilesetSheet;
    SpriteSheet m_marioSheet;
    std::string m_fontCharMap;

    void loadAssets();
    void drawText(SDL_Renderer* renderer, const std::string& text, int x, int y);
    void drawGround(SDL_Renderer* renderer);
};
