#pragma once

#include "entities/Entity.h"

// Starman power-up. Bounces continuously while moving right.
class Starman : public Entity {
public:
    Starman();

    void update(const Tilemap& tilemap) override;

    void startEmerge(float blockX, float blockY);
    bool isEmerging() const { return m_emerging; }

private:
    bool m_emerging = false;
    int m_emergeTimer = 0;
    float m_emergeStartY = 0;
    float m_emergeEndY = 0;
    float m_bounceVelocity = -4.0f;  // Bounce strength
};
