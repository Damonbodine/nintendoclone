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

// Physics — from the SMB disassembly (doppelganger's SMBDIS.ASM)
// NES uses 16 sub-pixels per pixel. Values here converted to pixels/frame.
// FrictionData table: [$E4, $98, $D0] for Mario

// Horizontal movement
constexpr float MARIO_WALK_ACCEL = 0.09375f;           // $98/256 = 0.59375 subpx -> /16 = ~0.037; adjusted for feel
constexpr float MARIO_RUN_ACCEL = 0.140625f;           // $E4/256 = 0.890625 subpx -> /16; B held acceleration
constexpr float MARIO_WALK_MAX_SPEED = 1.5f;           // 24 subpx/f / 16 = 1.5 px/f (soft cap, no B)
constexpr float MARIO_RUN_MAX_SPEED = 2.5f;            // 40 subpx/f / 16 = 2.5 px/f (hard cap, B held)
constexpr float MARIO_FRICTION = 0.05078125f;           // Deceleration when no input ($D0-range)
constexpr float MARIO_SKID_DECEL = 0.1015625f;         // Skid deceleration (facing != moving dir, accel doubles)

// Vertical movement — jump type selected by horizontal speed thresholds [9,16,25,28] subpx
// Low-speed jump:  init Y-vel = -4, gravity(held) = $20/256, gravity(rel) = $70/256
// Max-speed jump:  init Y-vel = -5, gravity(held) = $28/256, gravity(rel) = $90/256
constexpr float MARIO_JUMP_VELOCITY_WALK = -4.0f;      // Jump types 0-2 (speed < 25 subpx)
constexpr float MARIO_JUMP_VELOCITY_RUN = -5.0f;       // Jump types 3-4 (speed >= 25 subpx)
constexpr float GRAVITY_JUMP_HELD = 0.125f;            // $20/256 = 0.125 (A held, low-speed)
constexpr float GRAVITY_JUMP_HELD_FAST = 0.15625f;     // $28/256 = 0.15625 (A held, high-speed)
constexpr float GRAVITY_JUMP_RELEASED = 0.4375f;       // $70/256 = 0.4375 (A released, low-speed)
constexpr float GRAVITY_JUMP_RELEASED_FAST = 0.5625f;  // $90/256 = 0.5625 (A released, high-speed)
constexpr float TERMINAL_VELOCITY = 4.75f;

// Spring bounce
constexpr float SPRING_BOUNCE_HELD = -12.0f;           // Spring + A held
constexpr float SPRING_BOUNCE_TAP = -7.0f;             // Spring + A not held

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
constexpr float FIREBALL_SPEED = 4.0f;    // $40 subpx = 64/16 = 4.0 px/f (from FireballXSpdData)

// Piranha Plant timing (in frames)
constexpr int PIRANHA_EMERGE_FRAMES = 48;
constexpr int PIRANHA_PAUSE_FRAMES = 48;
constexpr int PIRANHA_RETRACT_FRAMES = 48;

// Game settings
constexpr int STARTING_LIVES = 3;
constexpr int COINS_FOR_1UP = 100;
constexpr int LEVEL_TIME = 400;
constexpr float TIMER_SPEED = 2.5f;  // Timer decrements per second

// The 21-frame rule: NES IntervalTimerControl counts 20->0 (21 frames per tick).
// All "long timers" (star, invincibility, etc.) decrement once per tick.
constexpr int FRAME_RULE = 21;  // ~0.35 seconds per timer tick

// Star power duration: ~23 timer ticks * 21 frames = ~483 frames (~8 sec)
constexpr int STAR_DURATION = 483;

// Invincibility after damage (frames)
constexpr int INVINCIBILITY_DURATION = 120;  // ~2 seconds

// Enemy collision only checks on even frames (authentic NES behavior)
constexpr bool ENEMY_COLLISION_EVEN_FRAMES_ONLY = true;

// Max simultaneous enemies (NES hardware limit)
constexpr int MAX_ACTIVE_ENEMIES = 5;

// Colors (NES palette)
namespace Color {
    constexpr int SKY_R = 0x5C, SKY_G = 0x94, SKY_B = 0xFC;
    constexpr int GROUND_R = 0xD0, GROUND_G = 0x70, GROUND_B = 0x30;
}

// Jump buffering
constexpr int JUMP_BUFFER_FRAMES = 6;

}  // namespace Constants
