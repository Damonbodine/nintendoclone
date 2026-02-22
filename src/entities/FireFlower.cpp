#include "entities/FireFlower.h"

FireFlower::FireFlower() {
    boxWidth = 14;
    boxHeight = 14;
    boxOffsetX = 1;
    boxOffsetY = 2;
    active = true;
}

void FireFlower::startEmerge(float blockX, float blockY) {
    x = blockX;
    m_emergeStartY = blockY;
    m_emergeEndY = blockY - 16;
    y = m_emergeStartY;
    m_emerging = true;
    m_emergeTimer = 0;
}

void FireFlower::update(const Tilemap& /* tilemap */) {
    if (!alive || !active) return;

    if (m_emerging) {
        m_emergeTimer++;
        float progress = static_cast<float>(m_emergeTimer) / 16.0f;

        if (progress >= 1.0f) {
            m_emerging = false;
            y = m_emergeEndY;
        } else {
            y = m_emergeStartY + (m_emergeEndY - m_emergeStartY) * progress;
        }
        return;
    }

    // Fire flower doesn't move, just animate
    sprite.update();
}
