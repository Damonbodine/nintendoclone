#pragma once

#include "entities/Entity.h"

// Super Mushroom (red) and 1-UP Mushroom (green).
// Spawns from ? blocks, moves right, bounces off walls, falls off edges.
class Mushroom : public Entity {
public:
    Mushroom(bool isOneUp = false);

    void update(const Tilemap& tilemap) override;

    // Start the emergence animation (rising out of block)
    void startEmerge(float blockX, float blockY);

    bool isOneUp() const { return m_isOneUp; }
    bool isEmerging() const { return m_emerging; }

private:
    bool m_isOneUp = false;
    bool m_emerging = false;
    int m_emergeTimer = 0;
    float m_emergeStartY = 0;
    float m_emergeEndY = 0;
};
