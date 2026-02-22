#include "states/TitleState.h"
#include "states/PlayState.h"
#include "core/Game.h"
#include "core/Constants.h"
#include <memory>
#include <fstream>

TitleState::TitleState(Game& game) : m_game(game) {}

void TitleState::enter() {
    m_blinkTimer = 0;
    loadAssets();
}

void TitleState::exit() {}

void TitleState::loadAssets() {
    auto& resources = m_game.getResources();

    SDL_Texture* fontTex = resources.loadTexture("assets/fonts/nes_font.png");
    if (fontTex) {
        m_fontSheet.init(fontTex, 8, 8);
    }

    SDL_Texture* tilesetTex = resources.loadTexture("assets/tiles/overworld_tileset.png");
    if (tilesetTex) {
        m_tilesetSheet.init(tilesetTex, 16, 16);
    }

    SDL_Texture* marioTex = resources.loadTexture("assets/sprites/mario_small.png");
    if (marioTex) {
        m_marioSheet.init(marioTex, 16, 16);
    }

    std::ifstream mapFile("assets/fonts/nes_font_map.txt");
    if (mapFile.is_open()) {
        std::getline(mapFile, m_fontCharMap);
    }
}

void TitleState::drawText(SDL_Renderer* renderer, const std::string& text, int x, int y) {
    if (!m_fontSheet.getTexture() || m_fontCharMap.empty()) return;

    for (size_t i = 0; i < text.size(); i++) {
        char ch = text[i];
        if (ch >= 'a' && ch <= 'z') ch = ch - 'a' + 'A';

        size_t idx = m_fontCharMap.find(ch);
        if (idx == std::string::npos) {
            continue;
        }

        SDL_Rect src = m_fontSheet.getFrameRect(static_cast<int>(idx));
        SDL_Rect dst = { x + static_cast<int>(i) * 8, y, 8, 8 };
        SDL_RenderCopy(renderer, m_fontSheet.getTexture(), &src, &dst);
    }
}

void TitleState::drawGround(SDL_Renderer* renderer) {
    if (!m_tilesetSheet.getTexture()) {
        // Fallback: colored rectangle
        SDL_SetRenderDrawColor(renderer, 0xD0, 0x70, 0x30, 255);
        SDL_Rect groundRect = { 0, Constants::NES_HEIGHT - 32, Constants::NES_WIDTH, 32 };
        SDL_RenderFillRect(renderer, &groundRect);
        return;
    }

    // Draw two rows of ground tiles at the bottom
    SDL_Rect groundSrc = m_tilesetSheet.getFrameRect(0);  // ground tile = frame 0
    for (int col = 0; col < Constants::NES_WIDTH / 16; col++) {
        SDL_Rect dst1 = { col * 16, Constants::NES_HEIGHT - 32, 16, 16 };
        SDL_RenderCopy(renderer, m_tilesetSheet.getTexture(), &groundSrc, &dst1);
        SDL_Rect dst2 = { col * 16, Constants::NES_HEIGHT - 16, 16, 16 };
        SDL_RenderCopy(renderer, m_tilesetSheet.getTexture(), &groundSrc, &dst2);
    }
}

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
    // === TITLE ===
    // Large title text rendered with the bitmap font (2x scale)
    // "SUPER" centered at top
    drawText(renderer, "SUPER", 88, 48);

    // "MARIO BROS." below
    drawText(renderer, "MARIO BROS.", 68, 64);

    // Small Mario sprite on the title screen
    if (m_marioSheet.getTexture()) {
        SDL_Rect marioSrc = m_marioSheet.getFrameRect(0);  // idle frame
        SDL_Rect marioDst = { 112, 100, 16, 16 };
        SDL_RenderCopy(renderer, m_marioSheet.getTexture(), &marioSrc, &marioDst);
    }

    // === MENU ===
    drawText(renderer, "1 PLAYER GAME", 72, 144);

    // "PRESS START" blinking
    if (m_showPressStart) {
        drawText(renderer, "PRESS START", 76, 168);
    }

    // === CREDITS ===
    drawText(renderer, "TOP- 000000", 72, 192);

    // === GROUND ===
    drawGround(renderer);
}
