#include "entities/Coin.h"

Coin::Coin() {
    boxWidth = 10;
    boxHeight = 14;
    boxOffsetX = 3;
    boxOffsetY = 1;
    active = true;
}

void Coin::update(const Tilemap& /* tilemap */) {
    if (!alive) return;

    // Coins just animate in place
    m_animTimer++;
    sprite.update();
}

void Coin::collect() {
    alive = false;
}
