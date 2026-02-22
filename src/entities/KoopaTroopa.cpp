#include "entities/KoopaTroopa.h"
#include "physics/Collision.h"
#include "world/Tilemap.h"
#include "core/Constants.h"

KoopaTroopa::KoopaTroopa() {
    boxWidth = 14;
    boxHeight = 24;  // Koopas are taller than Goombas
    boxOffsetX = 1;
    boxOffsetY = 8;  // Offset down from sprite top
    facingRight = false;
    vx = -Constants::KOOPA_SPEED;
}

void KoopaTroopa::update(const Tilemap& tilemap) {
    if (!alive || !active) return;

    // Handle flip death
    if (m_flipped) {
        m_flipVy += Constants::GRAVITY_JUMP_RELEASED;
        y += m_flipVy;
        if (y > 300) {
            alive = false;
        }
        return;
    }

    // Apply gravity
    applyGravity(Constants::GRAVITY_JUMP_RELEASED, Constants::TERMINAL_VELOCITY);

    float oldX = x;

    // Resolve collision
    resolveCollisionWithTilemap(tilemap);

    // Wall collision — reverse direction
    if (std::abs(x - oldX) < 0.01f && vx != 0 && m_state != KoopaState::SHELL_IDLE) {
        vx = -vx;
        facingRight = !facingRight;
    }

    // Fall death
    if (y > tilemap.getPixelHeight() + 32) {
        alive = false;
    }

    // Update animation
    switch (m_state) {
        case KoopaState::WALKING:
            sprite.setAnimation("walk");
            break;
        case KoopaState::SHELL_IDLE:
            sprite.setAnimation("shell_idle");
            break;
        case KoopaState::SHELL_MOVING:
            sprite.setAnimation("shell_spin");
            break;
    }

    sprite.update();
}

void KoopaTroopa::render(SDL_Renderer* renderer, float cameraX, float cameraY) {
    if (m_flipped) {
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

void KoopaTroopa::stomp() {
    if (m_state == KoopaState::WALKING) {
        m_state = KoopaState::SHELL_IDLE;
        vx = 0;
        boxHeight = 16;  // Shell is shorter
        boxOffsetY = 0;
        y += 8;  // Adjust position for shorter box
    } else if (m_state == KoopaState::SHELL_MOVING) {
        stopShell();
    }
}

void KoopaTroopa::kick(bool kickedRight) {
    m_state = KoopaState::SHELL_MOVING;
    vx = kickedRight ? Constants::SHELL_SPEED : -Constants::SHELL_SPEED;
    facingRight = kickedRight;
}

void KoopaTroopa::stopShell() {
    m_state = KoopaState::SHELL_IDLE;
    vx = 0;
}

void KoopaTroopa::killFlip() {
    m_flipped = true;
    m_flipVy = -3.0f;
    vx = 0;
}
