#pragma once

#include <SDL2/SDL.h>
#include "core/Constants.h"

class Window;

// Renders everything at NES resolution (256x240) to an internal texture,
// then scales that texture up to the window size. This gives us pixel-perfect
// NES graphics at any window resolution.
class Renderer {
public:
    Renderer();
    ~Renderer();

    bool init(Window& window);
    void shutdown();

    // Call at the start of each frame — clears the internal NES-resolution buffer
    void beginFrame();

    // Call at the end — scales the NES buffer to the window and presents
    void endFrame();

    // Get the SDL renderer for drawing operations (draws to NES-resolution target)
    SDL_Renderer* getSDLRenderer() const { return m_renderer; }

    // Get the internal render target texture
    SDL_Texture* getRenderTarget() const { return m_renderTarget; }

    // Draw a filled rectangle at NES resolution
    void drawRect(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255);

    // Draw a texture region at NES coordinates
    void drawTexture(SDL_Texture* texture, const SDL_Rect* src, const SDL_Rect* dst,
                     bool flipH = false);

    // Draw a texture with rotation
    void drawTextureEx(SDL_Texture* texture, const SDL_Rect* src, const SDL_Rect* dst,
                       double angle, SDL_RendererFlip flip);

private:
    SDL_Renderer* m_renderer = nullptr;
    SDL_Texture* m_renderTarget = nullptr;  // 256x240 internal buffer
};
