#include "states/TitleState.h"
#include "states/PlayState.h"
#include "core/Game.h"
#include "core/Constants.h"
#include <memory>

TitleState::TitleState(Game& game) : m_game(game) {}

void TitleState::enter() {
    m_blinkTimer = 0;
}

void TitleState::exit() {}

void TitleState::update(Game& game) {
    m_blinkTimer++;
    if (m_blinkTimer % 30 == 0) {
        m_showPressStart = !m_showPressStart;
    }

    // Press Start to begin
    if (game.getInput().isPressed(Input::START) ||
        game.getInput().isPressed(Input::A)) {
        game.changeState(std::make_unique<PlayState>(game));
    }
}

void TitleState::render(SDL_Renderer* renderer) {
    // Sky background is already cleared by the renderer

    // SUPER MARIO BROS title (placeholder rectangles)
    // Will be replaced with actual title screen graphics
    SDL_SetRenderDrawColor(renderer, 228, 92, 16, 255);  // SMB orange-red

    // "SUPER" text area (placeholder)
    SDL_Rect superRect = { 40, 40, 176, 16 };
    SDL_RenderFillRect(renderer, &superRect);

    // "MARIO BROS." text area (placeholder)
    SDL_Rect marioRect = { 24, 64, 208, 24 };
    SDL_RenderFillRect(renderer, &marioRect);

    // Copyright text area
    SDL_SetRenderDrawColor(renderer, 252, 252, 252, 255);
    SDL_Rect copyrightRect = { 56, 120, 144, 8 };
    SDL_RenderFillRect(renderer, &copyrightRect);

    // "PRESS START" blinking text
    if (m_showPressStart) {
        SDL_SetRenderDrawColor(renderer, 252, 252, 252, 255);
        SDL_Rect startRect = { 72, 160, 112, 8 };
        SDL_RenderFillRect(renderer, &startRect);
    }

    // Ground at bottom
    SDL_SetRenderDrawColor(renderer, 0xD0, 0x70, 0x30, 255);
    SDL_Rect groundRect = { 0, Constants::NES_HEIGHT - 32, Constants::NES_WIDTH, 32 };
    SDL_RenderFillRect(renderer, &groundRect);
}
