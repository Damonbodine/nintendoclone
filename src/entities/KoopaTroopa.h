#pragma once

#include "entities/Entity.h"

enum class KoopaState {
    WALKING,
    SHELL_IDLE,     // Shell stopped after stomp
    SHELL_MOVING    // Shell sliding after kick
};

class KoopaTroopa : public Entity {
public:
    KoopaTroopa();

    void update(const Tilemap& tilemap) override;
    void render(SDL_Renderer* renderer, float cameraX, float cameraY) override;

    // Called when Mario stomps
    void stomp();

    // Called when Mario walks into the idle shell
    void kick(bool kickedRight);

    // Called when shell hits the stomped shell again
    void stopShell();

    // Called when killed by fireball or star
    void killFlip();

    KoopaState getKoopaState() const { return m_state; }
    bool isShellMoving() const { return m_state == KoopaState::SHELL_MOVING; }
    bool isFlipped() const { return m_flipped; }

private:
    KoopaState m_state = KoopaState::WALKING;
    bool m_flipped = false;
    float m_flipVy = 0;
};
