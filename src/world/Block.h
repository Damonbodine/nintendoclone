#pragma once

#include "world/Tile.h"

// Tracks the state of interactive blocks and their visual effects.
// Used for brick break particle effects.
struct BrickParticle {
    float x, y;
    float vx, vy;
    int timer;
    bool active;
};

// Represents the particle effect when a brick breaks
struct BrickBreakEffect {
    BrickParticle particles[4];  // 4 quarter-brick chunks
    bool active = false;

    void spawn(float worldX, float worldY) {
        active = true;
        // Top-left chunk
        particles[0] = { worldX, worldY, -1.5f, -4.0f, 60, true };
        // Top-right chunk
        particles[1] = { worldX + 8, worldY, 1.5f, -4.0f, 60, true };
        // Bottom-left chunk
        particles[2] = { worldX, worldY + 8, -1.0f, -3.0f, 60, true };
        // Bottom-right chunk
        particles[3] = { worldX + 8, worldY + 8, 1.0f, -3.0f, 60, true };
    }

    void update() {
        if (!active) return;
        bool anyActive = false;
        for (auto& p : particles) {
            if (!p.active) continue;
            p.x += p.vx;
            p.y += p.vy;
            p.vy += 0.3f;  // Gravity
            p.timer--;
            if (p.timer <= 0) {
                p.active = false;
            } else {
                anyActive = true;
            }
        }
        if (!anyActive) active = false;
    }
};

// Coin pop animation when hitting a ? block
struct CoinPopEffect {
    float x, y;
    float vy;
    int timer;
    bool active = false;

    void spawn(float worldX, float worldY) {
        x = worldX;
        y = worldY - 16;  // Start above the block
        vy = -6.0f;
        timer = 30;
        active = true;
    }

    void update() {
        if (!active) return;
        y += vy;
        vy += 0.4f;  // Gravity
        timer--;
        if (timer <= 0) active = false;
    }
};
