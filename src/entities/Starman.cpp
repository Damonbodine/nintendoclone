#include "entities/Starman.h"
#include "core/Constants.h"
#include "world/Tilemap.h"

Starman::Starman() {
    boxWidth = 14;
    boxHeight = 14;
    boxOffsetX = 1;
    boxOffsetY = 2;
    facingRight = true;
    active = true;
}

void Starman::startEmerge(float blockX, float blockY) {
    x = blockX;
    m_emergeStartY = blockY;
    m_emergeEndY = blockY - 16;
    y = m_emergeStartY;
    m_emerging = true;
    m_emergeTimer = 0;
}

void Starman::update(const Tilemap& tilemap) {
    if (!alive || !active) return;

    if (m_emerging) {
        m_emergeTimer++;
        float progress = static_cast<float>(m_emergeTimer) / 16.0f;

        if (progress >= 1.0f) {
            m_emerging = false;
            y = m_emergeEndY;
            vx = Constants::MUSHROOM_SPEED;
            vy = m_bounceVelocity;
        } else {
            y = m_emergeStartY + (m_emergeEndY - m_emergeStartY) * progress;
        }
        return;
    }

    // Starman bounces continuously
    applyGravity(Constants::GRAVITY_JUMP_RELEASED, Constants::TERMINAL_VELOCITY);

    float oldX = x;
    bool wasOnGround = onGround;

    resolveCollisionWithTilemap(tilemap);

    // Bounce when landing
    if (onGround && !wasOnGround) {
        vy = m_bounceVelocity;
        onGround = false;
    }

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
