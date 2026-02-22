#pragma once

#include "core/Constants.h"
#include <algorithm>

// SMB-style camera that only scrolls right (never left).
// Follows Mario with a dead zone on the left side of the screen.
class Camera {
public:
    Camera() = default;

    void setLevelBounds(int levelPixelWidth, int levelPixelHeight) {
        m_levelWidth = levelPixelWidth;
        m_levelHeight = levelPixelHeight;
    }

    // Update camera position based on Mario's position.
    // The camera tracks Mario but never scrolls left.
    void update(float marioX, float marioY) {
        // Mario should be roughly 1/3 from the left edge of the screen
        float targetX = marioX - Constants::NES_WIDTH / 3.0f;

        // Camera only scrolls right — never go left of current position
        if (targetX > m_x) {
            m_x = targetX;
        }

        // Clamp to level bounds
        m_x = std::max(0.0f, m_x);
        m_x = std::min(m_x, static_cast<float>(m_levelWidth - Constants::NES_WIDTH));

        // Y is fixed in SMB (no vertical scrolling in overworld levels)
        m_y = 0;
    }

    float getX() const { return m_x; }
    float getY() const { return m_y; }

    // Reset camera to start of level
    void reset() {
        m_x = 0;
        m_y = 0;
    }

private:
    float m_x = 0;
    float m_y = 0;
    int m_levelWidth = 0;
    int m_levelHeight = 0;
};
