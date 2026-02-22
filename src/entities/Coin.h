#pragma once

#include "entities/Entity.h"

// Static coin floating in the air (collect on touch).
class Coin : public Entity {
public:
    Coin();

    void update(const Tilemap& tilemap) override;

    void collect();

private:
    int m_animTimer = 0;
};
