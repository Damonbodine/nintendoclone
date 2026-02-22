#pragma once

#include "entities/Entity.h"

class Goomba : public Entity {
public:
    Goomba();

    void update(const Tilemap& tilemap) override;
    void render(SDL_Renderer* renderer, float cameraX, float cameraY) override;

    // Called when Mario stomps this Goomba
    void stomp();

    // Called when killed by fireball, shell, or star
    void killFlip();

    bool isSquished() const { return m_squished; }

private:
    bool m_squished = false;
    int m_squishTimer = 0;    // Frames showing squished sprite before despawn
    bool m_flipped = false;   // Killed by fireball/shell — flip upside-down
    float m_flipVy = 0;       // Vertical velocity for flip death
};
