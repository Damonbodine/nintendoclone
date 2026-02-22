#pragma once

#include "entities/Entity.h"

// Mario's fireball projectile. Moves forward and bounces off the ground.
class Fireball : public Entity {
public:
    Fireball();

    void update(const Tilemap& tilemap) override;

    // Spawn at Mario's position, moving in the given direction
    void spawn(float startX, float startY, bool movingRight);

    // Called when fireball hits an enemy
    void explode();

private:
    bool m_exploding = false;
    int m_explodeTimer = 0;
    float m_bounceVelocity = -2.5f;
};
