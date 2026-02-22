#pragma once

#include "entities/Entity.h"
#include "core/Constants.h"

enum class PiranhaState {
    HIDDEN,
    EMERGING,
    PAUSED,
    RETRACTING
};

class PiranhaPlant : public Entity {
public:
    PiranhaPlant();

    void update(const Tilemap& tilemap) override;
    void render(SDL_Renderer* renderer, float cameraX, float cameraY) override;

    // Set the pipe position this plant lives in
    void setPipeTop(float pipeX, float pipeY);

    // Check if Mario is too close (prevents emergence)
    void setMarioNearby(bool nearby) { m_marioNearby = nearby; }

    // Kill with fireball or star
    void kill();

private:
    PiranhaState m_state = PiranhaState::HIDDEN;
    int m_timer = 0;
    float m_pipeX = 0;
    float m_pipeTopY = 0;
    float m_fullyHiddenY = 0;  // Y position when fully retracted
    float m_fullyShownY = 0;   // Y position when fully emerged
    bool m_marioNearby = false;
};
