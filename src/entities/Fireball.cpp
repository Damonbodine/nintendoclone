#include "entities/Fireball.h"
#include "core/Constants.h"
#include "world/Tilemap.h"

Fireball::Fireball() {
    boxWidth = 8;
    boxHeight = 8;
    boxOffsetX = 4;
    boxOffsetY = 4;
    active = true;
}

void Fireball::spawn(float startX, float startY, bool movingRight) {
    x = startX;
    y = startY;
    vx = movingRight ? Constants::FIREBALL_SPEED : -Constants::FIREBALL_SPEED;
    vy = 0;
    facingRight = movingRight;
    alive = true;
}

void Fireball::update(const Tilemap& tilemap) {
    if (!alive) return;

    if (m_exploding) {
        m_explodeTimer--;
        if (m_explodeTimer <= 0) {
            alive = false;
        }
        sprite.update();
        return;
    }

    // Apply gravity
    applyGravity(Constants::GRAVITY_JUMP_RELEASED, Constants::TERMINAL_VELOCITY);

    bool wasOnGround = onGround;
    float oldX = x;

    resolveCollisionWithTilemap(tilemap);

    // Bounce when hitting ground
    if (onGround && !wasOnGround) {
        vy = m_bounceVelocity;
        onGround = false;
    }

    // Die when hitting a wall
    if (std::abs(x - oldX) < 0.01f && vx != 0) {
        explode();
    }

    // Off screen death
    if (y > tilemap.getPixelHeight() + 32) {
        alive = false;
    }

    sprite.update();
}

void Fireball::explode() {
    m_exploding = true;
    m_explodeTimer = 8;
    vx = 0;
    vy = 0;
    sprite.setAnimation("explode");
}
