#include "entities/PiranhaPlant.h"

PiranhaPlant::PiranhaPlant() {
    boxWidth = 14;
    boxHeight = 24;
    boxOffsetX = 1;
    boxOffsetY = 0;
}

void PiranhaPlant::setPipeTop(float pipeX, float pipeY) {
    m_pipeX = pipeX;
    m_pipeTopY = pipeY;
    m_fullyHiddenY = pipeY + 32;  // Hidden inside the pipe
    m_fullyShownY = pipeY - 8;    // Poking out above the pipe
    x = pipeX + 1;                // Centered in pipe
    y = m_fullyHiddenY;
}

void PiranhaPlant::update(const Tilemap& /* tilemap */) {
    if (!alive || !active) return;

    m_timer++;

    switch (m_state) {
        case PiranhaState::HIDDEN:
            y = m_fullyHiddenY;
            // Wait before emerging, and don't emerge if Mario is nearby
            if (m_timer >= Constants::PIRANHA_PAUSE_FRAMES && !m_marioNearby) {
                m_state = PiranhaState::EMERGING;
                m_timer = 0;
            }
            break;

        case PiranhaState::EMERGING: {
            float progress = static_cast<float>(m_timer) / Constants::PIRANHA_EMERGE_FRAMES;
            if (progress >= 1.0f) {
                progress = 1.0f;
                m_state = PiranhaState::PAUSED;
                m_timer = 0;
            }
            y = m_fullyHiddenY + (m_fullyShownY - m_fullyHiddenY) * progress;
            break;
        }

        case PiranhaState::PAUSED:
            y = m_fullyShownY;
            if (m_timer >= Constants::PIRANHA_PAUSE_FRAMES) {
                m_state = PiranhaState::RETRACTING;
                m_timer = 0;
            }
            break;

        case PiranhaState::RETRACTING: {
            float progress = static_cast<float>(m_timer) / Constants::PIRANHA_RETRACT_FRAMES;
            if (progress >= 1.0f) {
                progress = 1.0f;
                m_state = PiranhaState::HIDDEN;
                m_timer = 0;
            }
            y = m_fullyShownY + (m_fullyHiddenY - m_fullyShownY) * progress;
            break;
        }
    }

    sprite.update();
}

void PiranhaPlant::render(SDL_Renderer* renderer, float cameraX, float cameraY) {
    if (!alive || !active) return;

    // Only render the visible portion (clip to pipe opening)
    // For simplicity, we render the full sprite and rely on the pipe tiles
    // visually covering the hidden part. A proper implementation would use
    // SDL clip rects.
    Entity::render(renderer, cameraX, cameraY);
}

void PiranhaPlant::kill() {
    alive = false;
}
