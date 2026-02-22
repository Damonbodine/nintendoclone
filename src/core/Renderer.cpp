#include "core/Renderer.h"
#include "core/Window.h"
#include <cstdio>

Renderer::Renderer() {}

Renderer::~Renderer() {
    shutdown();
}

bool Renderer::init(Window& window) {
    m_renderer = SDL_CreateRenderer(
        window.getSDLWindow(), -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!m_renderer) {
        std::fprintf(stderr, "Failed to create renderer: %s\n", SDL_GetError());
        return false;
    }

    // Create the internal NES-resolution render target
    m_renderTarget = SDL_CreateTexture(
        m_renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        Constants::NES_WIDTH,
        Constants::NES_HEIGHT
    );

    if (!m_renderTarget) {
        std::fprintf(stderr, "Failed to create render target: %s\n", SDL_GetError());
        return false;
    }

    // Use nearest-neighbor scaling for crisp pixel art
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    return true;
}

void Renderer::shutdown() {
    if (m_renderTarget) {
        SDL_DestroyTexture(m_renderTarget);
        m_renderTarget = nullptr;
    }
    if (m_renderer) {
        SDL_DestroyRenderer(m_renderer);
        m_renderer = nullptr;
    }
}

void Renderer::beginFrame() {
    // Set render target to our NES-resolution texture
    SDL_SetRenderTarget(m_renderer, m_renderTarget);

    // Clear with sky blue
    SDL_SetRenderDrawColor(m_renderer,
        Constants::Color::SKY_R,
        Constants::Color::SKY_G,
        Constants::Color::SKY_B, 255);
    SDL_RenderClear(m_renderer);
}

void Renderer::endFrame() {
    // Switch back to the default render target (the window)
    SDL_SetRenderTarget(m_renderer, nullptr);

    // Clear the window
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
    SDL_RenderClear(m_renderer);

    // Scale the NES buffer to fill the window, maintaining aspect ratio
    int windowW, windowH;
    SDL_GetRendererOutputSize(m_renderer, &windowW, &windowH);

    float scaleX = static_cast<float>(windowW) / Constants::NES_WIDTH;
    float scaleY = static_cast<float>(windowH) / Constants::NES_HEIGHT;
    float scale = std::min(scaleX, scaleY);

    int renderW = static_cast<int>(Constants::NES_WIDTH * scale);
    int renderH = static_cast<int>(Constants::NES_HEIGHT * scale);
    int offsetX = (windowW - renderW) / 2;
    int offsetY = (windowH - renderH) / 2;

    SDL_Rect destRect = { offsetX, offsetY, renderW, renderH };
    SDL_RenderCopy(m_renderer, m_renderTarget, nullptr, &destRect);

    SDL_RenderPresent(m_renderer);
}

void Renderer::drawRect(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(m_renderer, r, g, b, a);
    SDL_Rect rect = { x, y, w, h };
    SDL_RenderFillRect(m_renderer, &rect);
}

void Renderer::drawTexture(SDL_Texture* texture, const SDL_Rect* src, const SDL_Rect* dst,
                            bool flipH) {
    SDL_RendererFlip flip = flipH ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    SDL_RenderCopyEx(m_renderer, texture, src, dst, 0.0, nullptr, flip);
}

void Renderer::drawTextureEx(SDL_Texture* texture, const SDL_Rect* src, const SDL_Rect* dst,
                              double angle, SDL_RendererFlip flip) {
    SDL_RenderCopyEx(m_renderer, texture, src, dst, angle, nullptr, flip);
}
