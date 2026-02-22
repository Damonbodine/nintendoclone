#include "core/Game.h"
#include "core/Constants.h"
#include <cstdio>

Game::Game() {}

Game::~Game() {
    shutdown();
}

bool Game::init() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) < 0) {
        std::fprintf(stderr, "SDL init failed: %s\n", SDL_GetError());
        return false;
    }

    // Create window
    if (!m_window.init("Super Mario Bros", Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT)) {
        return false;
    }

    // Create renderer
    if (!m_renderer.init(m_window)) {
        return false;
    }

    // Init resource manager
    if (!m_resources.init(m_renderer.getSDLRenderer())) {
        return false;
    }

    // Init audio
    if (!m_audio.init()) {
        // Audio failure is non-fatal — game can run without sound
        std::fprintf(stderr, "Warning: Audio init failed, continuing without sound\n");
    }

    m_running = true;
    return true;
}

void Game::run() {
    while (m_running) {
        m_timer.tick();
        processEvents();

        // Fixed timestep updates
        while (m_timer.shouldUpdate()) {
            update();
        }

        render();

        // Apply pending state change (after update/render cycle)
        if (m_pendingState) {
            if (m_currentState) {
                m_currentState->exit();
            }
            m_currentState = std::move(m_pendingState);
            m_currentState->enter();
        }
    }
}

void Game::shutdown() {
    if (m_currentState) {
        m_currentState->exit();
        m_currentState.reset();
    }
    m_resources.shutdown();
    m_audio.shutdown();
    m_renderer.shutdown();
    m_window.shutdown();
    SDL_Quit();
}

void Game::changeState(std::unique_ptr<GameState> newState) {
    m_pendingState = std::move(newState);
}

void Game::processEvents() {
    m_input.beginFrame();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                m_running = false;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                    m_running = false;
                }
                m_input.handleEvent(event);
                break;
            case SDL_KEYUP:
                m_input.handleEvent(event);
                break;
            default:
                break;
        }
    }
}

void Game::update() {
    if (m_currentState) {
        m_currentState->update(*this);
    }
}

void Game::render() {
    m_renderer.beginFrame();

    if (m_currentState) {
        m_currentState->render(m_renderer.getSDLRenderer());
    }

    m_renderer.endFrame();
}
