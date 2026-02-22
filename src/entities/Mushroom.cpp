#include "entities/Mushroom.h"
#include "core/Constants.h"
#include "world/Tilemap.h"

Mushroom::Mushroom(bool isOneUp) : m_isOneUp(isOneUp) {
    boxWidth = 14;
    boxHeight = 14;
    boxOffsetX = 1;
    boxOffsetY = 2;
    facingRight = true;
    active = true;
}

void Mushroom::startEmerge(float blockX, float blockY) {
    x = blockX;
    m_emergeStartY = blockY;
    m_emergeEndY = blockY - 16;  // Rise one tile above the block
    y = m_emergeStartY;
    m_emerging = true;
    m_emergeTimer = 0;
    vx = 0;
}

void Mushroom::update(const Tilemap& tilemap) {
    if (!alive || !active) return;

    // Handle emergence animation
    if (m_emerging) {
        m_emergeTimer++;
        float progress = static_cast<float>(m_emergeTimer) / 16.0f;  // 16 frames to emerge

        if (progress >= 1.0f) {
            m_emerging = false;
            y = m_emergeEndY;
            vx = Constants::MUSHROOM_SPEED;  // Start moving right
        } else {
            y = m_emergeStartY + (m_emergeEndY - m_emergeStartY) * progress;
        }
        return;
    }

    // Normal movement
    applyGravity(Constants::GRAVITY_JUMP_RELEASED, Constants::TERMINAL_VELOCITY);

    float oldX = x;
    resolveCollisionWithTilemap(tilemap);

    // Bounce off walls
    if (std::abs(x - oldX) < 0.01f && vx != 0) {
        vx = -vx;
        facingRight = !facingRight;
    }

    // Fall death
    if (y > tilemap.getPixelHeight() + 32) {
        alive = false;
    }

    sprite.update();
}
