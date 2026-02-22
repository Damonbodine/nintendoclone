#pragma once

#include "entities/Entity.h"
#include "core/Input.h"
#include "core/Constants.h"

// Mario's power state
enum class PowerState {
    SMALL,
    BIG,
    FIRE
};

// Mario's movement/animation state
enum class MarioState {
    IDLE,
    WALK,
    RUN,
    SKID,
    JUMP,
    FALL,
    CROUCH,
    CLIMB,      // Flagpole
    DIE,
    GROW,       // Transitioning SMALL -> BIG
    SHRINK,     // Transitioning BIG -> SMALL
    PIPE_ENTER, // Entering a pipe
    PIPE_EXIT   // Exiting a pipe
};

class Mario : public Entity {
public:
    Mario();

    void update(const Tilemap& tilemap) override;
    void render(SDL_Renderer* renderer, float cameraX, float cameraY) override;

    // Call each frame with current input
    void handleInput(const Input& input);

    // Power state management
    void grow();              // SMALL -> BIG
    void getFire();           // BIG -> FIRE
    void takeDamage();        // Lose power level
    void die();               // Death
    void collectStar();       // Start invincibility

    // State queries
    PowerState getPowerState() const { return m_powerState; }
    MarioState getMarioState() const { return m_marioState; }
    bool isInvincible() const { return m_invincibilityTimer > 0; }
    bool hasStar() const { return m_starTimer > 0; }
    bool isDead() const { return m_marioState == MarioState::DIE; }
    bool isGrounded() const { return onGround; }

    // Pre-collision velocity (saved before tilemap resolution, for stomp/block checks)
    float preCollisionVy = 0;

    // Event flags (read and cleared by PlayState each frame for audio/effects)
    bool justJumped = false;
    bool justDied = false;

    // Score and stats
    int score = 0;
    int coins = 0;
    int lives = Constants::STARTING_LIVES;
    int stompCombo = 0;  // Consecutive stomp counter (resets on landing)

    // Fireball throwing
    bool canThrowFireball() const;
    void throwFireball();
    int fireballCount = 0;  // Active fireballs on screen

private:
    PowerState m_powerState = PowerState::SMALL;
    MarioState m_marioState = MarioState::IDLE;

    // Input state
    bool m_inputLeft = false;
    bool m_inputRight = false;
    bool m_inputJump = false;
    bool m_inputJumpPressed = false;
    bool m_inputRun = false;
    bool m_inputDown = false;

    // Jump mechanics
    bool m_jumpHeld = false;       // Is the jump button currently held?
    int m_jumpBufferTimer = 0;     // Frames since jump was pressed (for buffering)
    bool m_hasJumped = false;      // Prevent double-jumping

    // Invincibility
    int m_invincibilityTimer = 0;  // Damage invincibility frames
    int m_starTimer = 0;           // Star power frames

    // Transition animations
    int m_transitionTimer = 0;     // For grow/shrink animations
    int m_deathTimer = 0;          // Death animation timer
    float m_deathVy = 0;           // Death animation velocity

    // Physics update
    void updatePhysics(const Tilemap& tilemap);
    void updateHorizontalMovement();
    void updateJump();
    void updateAnimationState();
    void updateTimers();
    void updateCollisionBox();

    // Check for block hits (when jumping into blocks from below)
    void checkBlockHits(const Tilemap& tilemap);
};
