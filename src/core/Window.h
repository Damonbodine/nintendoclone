#pragma once

#include <SDL2/SDL.h>
#include <string>

class Window {
public:
    Window();
    ~Window();

    bool init(const std::string& title, int width, int height);
    void shutdown();

    SDL_Window* getSDLWindow() const { return m_window; }
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }

private:
    SDL_Window* m_window = nullptr;
    int m_width = 0;
    int m_height = 0;
};
