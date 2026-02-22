#pragma once

namespace Constants {

// NES display specs
constexpr int NES_WIDTH = 256;
constexpr int NES_HEIGHT = 240;
constexpr int SCALE_FACTOR = 3;
constexpr int WINDOW_WIDTH = NES_WIDTH * SCALE_FACTOR;
constexpr int WINDOW_HEIGHT = NES_HEIGHT * SCALE_FACTOR;

// Frame rate
constexpr int TARGET_FPS = 60;
constexpr double FRAME_TIME_MS = 1000.0 / TARGET_FPS;  // 16.6667ms

// Tile dimensions
constexpr int TILE_SIZE = 16;
constexpr int TILES_WIDE = NES_WIDTH / TILE_SIZE;   // 16 tiles visible
constexpr int TILES_HIGH = NES_HEIGHT / TILE_SIZE;   // 15 tiles visible

// Physics (values in pixels per frame at 60fps)
// Horizontal movement
constexpr float MARIO_WALK_ACCEL = 0.046875f;        // ~0x0098 in NES sub-pixels
constexpr float MARIO_RUN_ACCEL = 0.046875f;
constexpr float MARIO_WALK_MAX_SPEED = 1.5625f;      // ~0x0190
constexpr float MARIO_RUN_MAX_SPEED = 2.5f;           // ~0x0280
constexpr float MARIO_FRICTION = 0.05078125f;          // Deceleration when no input
constexpr float MARIO_SKID_DECEL = 0.1015625f;        // Deceleration when turning

// Vertical movement
constexpr float MARIO_JUMP_VELOCITY_WALK = -4.0f;     // Initial upward velocity (walking)
constexpr float MARIO_JUMP_VELOCITY_RUN = -5.0f;      // Initial upward velocity (running)
constexpr float GRAVITY_JUMP_HELD = 0.2f;             // Gravity while holding jump
constexpr float GRAVITY_JUMP_RELEASED = 0.7f;         // Gravity after releasing jump
constexpr float TERMINAL_VELOCITY = 4.75f;

// Small Mario collision box
constexpr int SMALL_MARIO_WIDTH = 12;
constexpr int SMALL_MARIO_HEIGHT = 16;

// Big Mario collision box
constexpr int BIG_MARIO_WIDTH = 12;
constexpr int BIG_MARIO_HEIGHT = 32;

// Enemy speeds
constexpr float GOOMBA_SPEED = 0.5f;
constexpr float KOOPA_SPEED = 0.5f;
constexpr float SHELL_SPEED = 3.0f;
constexpr float MUSHROOM_SPEED = 1.5f;
constexpr float FIREBALL_SPEED = 3.5f;

// Piranha Plant timing (in frames)
constexpr int PIRANHA_EMERGE_FRAMES = 48;
constexpr int PIRANHA_PAUSE_FRAMES = 48;
constexpr int PIRANHA_RETRACT_FRAMES = 48;

// Game settings
constexpr int STARTING_LIVES = 3;
constexpr int COINS_FOR_1UP = 100;
constexpr int LEVEL_TIME = 400;
constexpr float TIMER_SPEED = 2.5f;  // Timer decrements per second

// Star power duration (frames)
constexpr int STAR_DURATION = 600;  // 10 seconds at 60fps

// Invincibility after damage (frames)
constexpr int INVINCIBILITY_DURATION = 120;  // 2 seconds at 60fps

// Colors (NES palette)
namespace Color {
    constexpr int SKY_R = 0x5C, SKY_G = 0x94, SKY_B = 0xFC;
    constexpr int GROUND_R = 0xD0, GROUND_G = 0x70, GROUND_B = 0x30;
}

// Jump buffering
constexpr int JUMP_BUFFER_FRAMES = 6;

}  // namespace Constants
