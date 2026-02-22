#include "entities/Entity.h"
#include "physics/Collision.h"
#include "world/Tilemap.h"
#include "core/Constants.h"

Entity::Entity() {}

AABB Entity::getWorldBounds() const {
    return { x + boxOffsetX, y + boxOffsetY, boxWidth, boxHeight };
}

void Entity::render(SDL_Renderer* renderer, float cameraX, float cameraY) {
    if (!alive || !active) return;

    int screenX = static_cast<int>(x - cameraX);
    int screenY = static_cast<int>(y - cameraY);

    sprite.render(renderer, screenX, screenY, !facingRight);
}

void Entity::applyGravity(float gravity, float terminalVel) {
    vy += gravity;
    if (vy > terminalVel) {
        vy = terminalVel;
    }
}

void Entity::resolveCollisionWithTilemap(const Tilemap& tilemap) {
    AABB bounds = getWorldBounds();

    auto result = Collision::resolveWithTilemap(bounds, vx, vy, tilemap);

    // Update position from resolved collision
    x = result.resolvedX - boxOffsetX;
    y = result.resolvedY - boxOffsetY;

    // Update velocity based on what we hit
    if (result.hitLeft || result.hitRight) {
        vx = 0;
    }
    if (result.hitTop) {
        vy = 0;
    }
    if (result.hitBottom) {
        vy = 0;
        onGround = true;
    } else {
        onGround = false;
    }
}
