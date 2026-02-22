#include "core/Window.h"
#include <cstdio>

Window::Window() {}

Window::~Window() {
    shutdown();
}

bool Window::init(const std::string& title, int width, int height) {
    m_width = width;
    m_height = height;

    m_window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );

    if (!m_window) {
        std::fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

void Window::shutdown() {
    if (m_window) {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }
}
