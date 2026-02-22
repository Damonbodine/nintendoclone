#include "core/Timer.h"

Timer::Timer() {
    m_lastTime = SDL_GetPerformanceCounter();
    m_fpsTimer = SDL_GetTicks();
}

void Timer::tick() {
    Uint64 now = SDL_GetPerformanceCounter();
    double elapsed = static_cast<double>(now - m_lastTime) /
                     static_cast<double>(SDL_GetPerformanceFrequency()) * 1000.0;
    m_lastTime = now;

    // Cap accumulated time to prevent spiral of death
    if (elapsed > 100.0) {
        elapsed = 100.0;
    }

    m_accumulator += elapsed;

    // FPS counter
    m_fpsCounter++;
    Uint32 currentTicks = SDL_GetTicks();
    if (currentTicks - m_fpsTimer >= 1000) {
        m_fps = m_fpsCounter;
        m_fpsCounter = 0;
        m_fpsTimer = currentTicks;
    }
}

bool Timer::shouldUpdate() {
    if (m_accumulator >= Constants::FRAME_TIME_MS) {
        m_accumulator -= Constants::FRAME_TIME_MS;
        m_frameCount++;
        return true;
    }
    return false;
}

float Timer::getInterpolation() const {
    return static_cast<float>(m_accumulator / Constants::FRAME_TIME_MS);
}
