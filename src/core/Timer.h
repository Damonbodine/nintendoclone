#pragma once

#include <SDL2/SDL.h>
#include "core/Constants.h"

// Fixed timestep timer for consistent 60fps game loop.
// Accumulates real time and allows consuming it in fixed steps.
class Timer {
public:
    Timer();

    // Call at the top of the game loop — records elapsed time
    void tick();

    // Returns true if there's a fixed timestep to consume.
    // Call in a while loop: while(timer.shouldUpdate()) { update(); }
    bool shouldUpdate();

    // Get the interpolation factor for rendering between frames (0.0 - 1.0)
    float getInterpolation() const;

    // Get frames per second (for debug display)
    int getFPS() const { return m_fps; }

    // Get total elapsed frames since start
    Uint64 getFrameCount() const { return m_frameCount; }

private:
    Uint64 m_lastTime = 0;
    double m_accumulator = 0.0;
    Uint64 m_frameCount = 0;

    // FPS calculation
    int m_fps = 0;
    int m_fpsCounter = 0;
    Uint64 m_fpsTimer = 0;
};
