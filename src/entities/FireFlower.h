#pragma once

#include "entities/Entity.h"

// Fire Flower power-up. Spawns from ? blocks when Mario is already big.
// Does NOT move — stays on top of the block.
class FireFlower : public Entity {
public:
    FireFlower();

    void update(const Tilemap& tilemap) override;

    void startEmerge(float blockX, float blockY);
    bool isEmerging() const { return m_emerging; }

private:
    bool m_emerging = false;
    int m_emergeTimer = 0;
    float m_emergeStartY = 0;
    float m_emergeEndY = 0;
};
