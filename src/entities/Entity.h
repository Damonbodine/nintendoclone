#pragma once

#include "physics/AABB.h"
#include "graphics/Sprite.h"
#include <SDL2/SDL.h>

class Tilemap;
class Renderer;

// Base class for all game entities (Mario, enemies, items).
class Entity {
public:
    Entity();
    virtual ~Entity() = default;

    virtual void update(const Tilemap& tilemap) = 0;
    virtual void render(SDL_Renderer* renderer, float cameraX, float cameraY);

    // Position (sub-pixel precision)
    float x = 0, y = 0;

    // Velocity
    float vx = 0, vy = 0;

    // Bounding box (relative to position)
    AABB getWorldBounds() const;

    // State
    bool alive = true;
    bool active = false;    // Activated when camera approaches
    bool facingRight = true;
    bool onGround = false;

    // Sprite
    Sprite sprite;

    // Activation: entities spawn when camera gets close
    float spawnX = 0;  // Original spawn position
    float spawnY = 0;

    // Bounding box offset and size (relative to position)
    float boxOffsetX = 0;
    float boxOffsetY = 0;
    float boxWidth = 16;
    float boxHeight = 16;

protected:
    // Apply gravity and resolve tile collision
    void applyGravity(float gravity, float terminalVel);
    void resolveCollisionWithTilemap(const Tilemap& tilemap);
};
