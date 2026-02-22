#include "entities/Goomba.h"
#include "physics/Collision.h"
#include "world/Tilemap.h"
#include "core/Constants.h"

Goomba::Goomba() {
    boxWidth = 14;
    boxHeight = 14;
    boxOffsetX = 1;
    boxOffsetY = 2;
    facingRight = false;  // Goombas start walking left
    vx = -Constants::GOOMBA_SPEED;
}

void Goomba::update(const Tilemap& tilemap) {
    if (!alive || !active) return;

    // Handle squish animation
    if (m_squished) {
        m_squishTimer--;
        if (m_squishTimer <= 0) {
            alive = false;
        }
        return;
    }

    // Handle flip death (fireball/shell kill)
    if (m_flipped) {
        m_flipVy += Constants::GRAVITY_JUMP_RELEASED;
        y += m_flipVy;
        if (y > 300) {  // Off screen
            alive = false;
        }
        return;
    }

    // Apply gravity
    applyGravity(Constants::GRAVITY_JUMP_RELEASED, Constants::TERMINAL_VELOCITY);

    // Save old position for wall detection
    float oldX = x;

    // Resolve collision
    resolveCollisionWithTilemap(tilemap);

    // If we hit a wall, reverse direction
    if (x == oldX && vx != 0) {
        vx = -vx;
        facingRight = !facingRight;
    }

    // Fall death
    if (y > tilemap.getPixelHeight() + 32) {
        alive = false;
    }

    sprite.update();
}

void Goomba::render(SDL_Renderer* renderer, float cameraX, float cameraY) {
    if (!alive || !active) return;

    if (m_flipped) {
        // Render upside-down
        int screenX = static_cast<int>(x - cameraX);
        int screenY = static_cast<int>(y - cameraY);
        if (sprite.getCurrentAnimation()) {
            SDL_Texture* texture = sprite.getCurrentAnimation()->getTexture();
            SDL_Rect src = sprite.getCurrentAnimation()->getCurrentFrameRect();
            SDL_Rect dst = { screenX, screenY, src.w, src.h };
            SDL_RenderCopyEx(renderer, texture, &src, &dst, 180.0, nullptr, SDL_FLIP_NONE);
        }
        return;
    }

    Entity::render(renderer, cameraX, cameraY);
}

void Goomba::stomp() {
    m_squished = true;
    m_squishTimer = 30;  // Show squished sprite for 30 frames
    vx = 0;
    vy = 0;
    sprite.setAnimation("squished");
}

void Goomba::killFlip() {
    m_flipped = true;
    m_flipVy = -3.0f;  // Pop up
    vx = 0;
}
