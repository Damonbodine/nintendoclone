#pragma once

#include "core/Window.h"
#include "core/Renderer.h"
#include "core/Input.h"
#include "core/Timer.h"
#include "core/Audio.h"
#include "core/ResourceManager.h"
#include "states/GameState.h"
#include <memory>
#include <string>

// Top-level game class. Owns all core systems and runs the main loop.
class Game {
public:
    Game();
    ~Game();

    bool init();
    void run();
    void shutdown();

    // State management
    void changeState(std::unique_ptr<GameState> newState);

    // Accessors for subsystems
    Window& getWindow() { return m_window; }
    Renderer& getRenderer() { return m_renderer; }
    Input& getInput() { return m_input; }
    Audio& getAudio() { return m_audio; }
    ResourceManager& getResources() { return m_resources; }
    Timer& getTimer() { return m_timer; }

    void quit() { m_running = false; }
    bool isRunning() const { return m_running; }

private:
    Window m_window;
    Renderer m_renderer;
    Input m_input;
    Timer m_timer;
    Audio m_audio;
    ResourceManager m_resources;

    std::unique_ptr<GameState> m_currentState;
    std::unique_ptr<GameState> m_pendingState;

    bool m_running = false;

    void processEvents();
    void update();
    void render();
};
