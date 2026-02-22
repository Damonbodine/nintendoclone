#include "entities/Mario.h"
#include "physics/Collision.h"
#include "world/Tilemap.h"
#include <cmath>
#include <algorithm>

Mario::Mario() {
    alive = true;
    active = true;
    facingRight = true;

    updateCollisionBox();
}

void Mario::handleInput(const Input& input) {
    m_inputLeft = input.isHeld(Input::LEFT);
    m_inputRight = input.isHeld(Input::RIGHT);
    m_inputJump = input.isHeld(Input::A);
    m_inputJumpPressed = input.isPressed(Input::A);
    m_inputRun = input.isHeld(Input::B);
    m_inputDown = input.isHeld(Input::DOWN);
}

void Mario::update(const Tilemap& tilemap) {
    if (!alive) return;

    // Clear per-frame event flags
    justJumped = false;
    justDied = false;

    // Handle transition animations
    if (m_marioState == MarioState::GROW || m_marioState == MarioState::SHRINK) {
        m_transitionTimer--;
        if (m_transitionTimer <= 0) {
            m_marioState = MarioState::IDLE;
            updateCollisionBox();
        }
        return;
    }

    // Handle death animation
    if (m_marioState == MarioState::DIE) {
        m_deathTimer++;
        if (m_deathTimer < 30) {
            // Pause at top of death bounce
        } else {
            m_deathVy += Constants::GRAVITY_JUMP_RELEASED;
            y += m_deathVy;
        }
        return;
    }

    updateTimers();
    updateHorizontalMovement();
    updateJump();
    updatePhysics(tilemap);
    updateAnimationState();

    sprite.update();
}

void Mario::updateHorizontalMovement() {
    float maxSpeed = m_inputRun ? Constants::MARIO_RUN_MAX_SPEED : Constants::MARIO_WALK_MAX_SPEED;
    float accel = m_inputRun ? Constants::MARIO_RUN_ACCEL : Constants::MARIO_WALK_ACCEL;

    if (m_inputRight) {
        if (vx < 0) {
            // Skidding — turning around
            vx += Constants::MARIO_SKID_DECEL;
            if (vx > 0) vx = 0;
            m_marioState = MarioState::SKID;
        } else {
            vx += accel;
            if (vx > maxSpeed) {
                // When B is released, gradually decelerate from run speed to walk speed
                // instead of hard-capping immediately
                if (!m_inputRun && vx > Constants::MARIO_WALK_MAX_SPEED) {
                    vx -= Constants::MARIO_FRICTION;
                } else {
                    vx = maxSpeed;
                }
            }
        }
        facingRight = true;
    } else if (m_inputLeft) {
        if (vx > 0) {
            // Skidding
            vx -= Constants::MARIO_SKID_DECEL;
            if (vx < 0) vx = 0;
            m_marioState = MarioState::SKID;
        } else {
            vx -= accel;
            if (vx < -maxSpeed) {
                if (!m_inputRun && vx < -Constants::MARIO_WALK_MAX_SPEED) {
                    vx += Constants::MARIO_FRICTION;
                } else {
                    vx = -maxSpeed;
                }
            }
        }
        facingRight = false;
    } else {
        // No horizontal input — apply friction
        if (vx > 0) {
            vx -= Constants::MARIO_FRICTION;
            if (vx < 0) vx = 0;
        } else if (vx < 0) {
            vx += Constants::MARIO_FRICTION;
            if (vx > 0) vx = 0;
        }
    }

    // Crouch (big Mario only)
    if (m_inputDown && onGround && m_powerState != PowerState::SMALL) {
        m_marioState = MarioState::CROUCH;
        // Kill horizontal velocity while crouching
        if (vx > 0) {
            vx -= Constants::MARIO_FRICTION;
            if (vx < 0) vx = 0;
        } else if (vx < 0) {
            vx += Constants::MARIO_FRICTION;
            if (vx > 0) vx = 0;
        }
    }
}

void Mario::updateJump() {
    // Jump buffering: remember if jump was pressed recently
    if (m_inputJumpPressed) {
        m_jumpBufferTimer = Constants::JUMP_BUFFER_FRAMES;
    }
    if (m_jumpBufferTimer > 0) {
        m_jumpBufferTimer--;
    }

    // Start jump
    if (m_jumpBufferTimer > 0 && onGround && !m_hasJumped) {
        float absSpeed = std::abs(vx);
        float jumpVel;

        // Jump velocity depends on horizontal speed
        if (absSpeed >= Constants::MARIO_RUN_MAX_SPEED * 0.8f) {
            jumpVel = Constants::MARIO_JUMP_VELOCITY_RUN;
        } else {
            jumpVel = Constants::MARIO_JUMP_VELOCITY_WALK;
        }

        vy = jumpVel;
        onGround = false;
        m_jumpHeld = true;
        m_hasJumped = true;
        m_jumpBufferTimer = 0;
        stompCombo = 0;
        justJumped = true;
    }

    // Variable-height jump: if button is held, use low gravity
    if (!m_inputJump) {
        m_jumpHeld = false;
    }

    // Reset jump ability when landing
    if (onGround) {
        m_hasJumped = false;
    }
}

void Mario::updatePhysics(const Tilemap& tilemap) {
    // Apply gravity — NES uses speed-dependent gravity values
    // Low-speed jump (types 0-2): held=$20, released=$70
    // High-speed jump (types 3-4): held=$28, released=$90
    bool isFastJump = std::abs(vx) >= Constants::MARIO_RUN_MAX_SPEED * 0.7f;
    float gravity;
    if (vy < 0 && m_jumpHeld) {
        gravity = isFastJump ? Constants::GRAVITY_JUMP_HELD_FAST
                             : Constants::GRAVITY_JUMP_HELD;
    } else {
        gravity = isFastJump ? Constants::GRAVITY_JUMP_RELEASED_FAST
                             : Constants::GRAVITY_JUMP_RELEASED;
    }

    applyGravity(gravity, Constants::TERMINAL_VELOCITY);

    // Save pre-collision velocity for stomp and block hit detection in PlayState
    preCollisionVy = vy;

    // Resolve collision with tilemap
    resolveCollisionWithTilemap(tilemap);

    // Fall death check
    if (y > tilemap.getPixelHeight() + 32) {
        die();
    }

    // Don't go past left edge of level
    if (x < 0) {
        x = 0;
        vx = 0;
    }
}

void Mario::updateAnimationState() {
    if (m_marioState == MarioState::GROW ||
        m_marioState == MarioState::SHRINK ||
        m_marioState == MarioState::DIE ||
        m_marioState == MarioState::CROUCH) {
        // Don't override transition states
        // (CROUCH was already set in updateHorizontalMovement if applicable)
        if (m_marioState == MarioState::CROUCH && !m_inputDown) {
            m_marioState = MarioState::IDLE;
        }
        return;
    }

    if (!onGround) {
        m_marioState = (vy < 0) ? MarioState::JUMP : MarioState::FALL;
    } else if (std::abs(vx) > 0.1f) {
        if (std::abs(vx) >= Constants::MARIO_RUN_MAX_SPEED * 0.8f && m_inputRun) {
            m_marioState = MarioState::RUN;
        } else {
            m_marioState = MarioState::WALK;
        }
    } else {
        if (m_marioState != MarioState::SKID) {
            m_marioState = MarioState::IDLE;
        }
    }

    // Update animation name on sprite
    // Big/Fire Mario uses "big_" prefixed animation names
    std::string baseName;
    switch (m_marioState) {
        case MarioState::IDLE:  baseName = "idle"; break;
        case MarioState::WALK:  baseName = "walk"; break;
        case MarioState::RUN:   baseName = "walk"; break;
        case MarioState::SKID:  baseName = "skid"; break;
        case MarioState::JUMP:  baseName = "jump"; break;
        case MarioState::FALL:  baseName = "jump"; break;
        case MarioState::CROUCH: baseName = "crouch"; break;
        case MarioState::CLIMB: baseName = "climb"; break;
        case MarioState::DIE:   baseName = "die"; break;
        default: baseName = "idle"; break;
    }

    std::string animName = (m_powerState != PowerState::SMALL)
        ? "big_" + baseName : baseName;
    sprite.setAnimation(animName);

    // Scale walk animation speed with velocity
    if (m_marioState == MarioState::WALK || m_marioState == MarioState::RUN) {
        Animation* anim = sprite.getCurrentAnimation();
        if (anim) {
            float speed = std::abs(vx);
            int frameDuration;
            if (speed < 0.8f) {
                frameDuration = 10;
            } else if (speed < 1.5f) {
                frameDuration = 6;
            } else if (speed < 2.2f) {
                frameDuration = 4;
            } else {
                frameDuration = 2;
            }
            anim->setFrameDuration(frameDuration);
        }
    }
}

void Mario::updateTimers() {
    if (m_invincibilityTimer > 0) {
        m_invincibilityTimer--;
        // Flicker visibility during invincibility
        sprite.setVisible((m_invincibilityTimer / 2) % 2 == 0);
        if (m_invincibilityTimer == 0) {
            sprite.setVisible(true);
        }
    }

    if (m_starTimer > 0) {
        m_starTimer--;
    }
}

void Mario::updateCollisionBox() {
    if (m_powerState == PowerState::SMALL) {
        boxWidth = Constants::SMALL_MARIO_WIDTH;
        boxHeight = Constants::SMALL_MARIO_HEIGHT;
        boxOffsetX = 2;  // Center 12px box in 16px sprite
        boxOffsetY = 0;
    } else {
        boxWidth = Constants::BIG_MARIO_WIDTH;
        boxHeight = Constants::BIG_MARIO_HEIGHT;
        boxOffsetX = 2;
        boxOffsetY = 0;
    }
}

void Mario::render(SDL_Renderer* renderer, float cameraX, float cameraY) {
    if (!alive && m_marioState != MarioState::DIE) return;

    // Star power palette cycling effect
    if (m_starTimer > 0) {
        // TODO: Implement palette cycling via color modulation
        int cycle = (m_starTimer / 4) % 3;
        switch (cycle) {
            case 0: SDL_SetTextureColorMod(sprite.getCurrentAnimation()->getTexture(), 255, 255, 255); break;
            case 1: SDL_SetTextureColorMod(sprite.getCurrentAnimation()->getTexture(), 100, 255, 100); break;
            case 2: SDL_SetTextureColorMod(sprite.getCurrentAnimation()->getTexture(), 255, 100, 100); break;
        }
    }

    Entity::render(renderer, cameraX, cameraY);

    // Reset color mod
    if (m_starTimer > 0 && sprite.getCurrentAnimation()) {
        SDL_SetTextureColorMod(sprite.getCurrentAnimation()->getTexture(), 255, 255, 255);
    }
}

void Mario::grow() {
    if (m_powerState == PowerState::SMALL) {
        m_powerState = PowerState::BIG;
        m_marioState = MarioState::GROW;
        m_transitionTimer = 48;  // ~0.8 seconds of growth animation
        // Adjust Y position (Mario grows upward)
        y -= 16;
        updateCollisionBox();
    }
}

void Mario::getFire() {
    if (m_powerState == PowerState::BIG) {
        m_powerState = PowerState::FIRE;
        // Brief palette swap animation
        m_transitionTimer = 16;
        m_marioState = MarioState::GROW;  // Reuse grow state for transition
    }
}

void Mario::takeDamage() {
    if (m_invincibilityTimer > 0 || m_starTimer > 0) return;

    if (m_powerState == PowerState::FIRE) {
        m_powerState = PowerState::BIG;
        m_invincibilityTimer = Constants::INVINCIBILITY_DURATION;
    } else if (m_powerState == PowerState::BIG) {
        m_powerState = PowerState::SMALL;
        m_marioState = MarioState::SHRINK;
        m_transitionTimer = 48;
        y += 16;  // Shrink downward
        m_invincibilityTimer = Constants::INVINCIBILITY_DURATION;
        updateCollisionBox();
    } else {
        die();
    }
}

void Mario::die() {
    if (m_marioState == MarioState::DIE) return;

    m_marioState = MarioState::DIE;
    alive = false;
    lives--;
    vx = 0;
    vy = 0;
    m_deathVy = Constants::MARIO_JUMP_VELOCITY_WALK;
    m_deathTimer = 0;
    justDied = true;

    sprite.setAnimation("die");
}

void Mario::collectStar() {
    m_starTimer = Constants::STAR_DURATION;
}

bool Mario::canThrowFireball() const {
    return m_powerState == PowerState::FIRE && fireballCount < 2;
}

void Mario::throwFireball() {
    if (canThrowFireball()) {
        fireballCount++;
        // Actual fireball spawning handled by the game state
    }
}
