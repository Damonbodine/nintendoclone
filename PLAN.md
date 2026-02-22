# Super Mario Bros Clone — Comprehensive Build Plan

## Project Overview

A faithful recreation of **Super Mario Bros (NES, 1985)** starting with World 1-1,
built to replicate the original game's graphics, physics, mechanics, and feel at a
pixel-accurate level. This is not a "Mario-like" — it's a clone that should be
indistinguishable from the original in gameplay.

---

## Architecture Decision: C++ 17 with SDL2

### Why C++ / SDL2

| Concern | Decision |
|---|---|
| **Rendering control** | SDL2 gives us a raw pixel buffer. We render at NES resolution (256×240) then scale up — no engine fighting us. |
| **Physics accuracy** | We need fixed-point sub-pixel math matching the NES. C++ gives us full control with zero abstraction overhead. |
| **Input latency** | SDL2 polls input directly. No engine event queue sitting between the player and the game loop. |
| **Audio** | SDL2_mixer handles WAV/OGG playback for sound effects and music. |
| **Cross-platform** | SDL2 compiles on Windows, macOS, Linux. Can target WebAssembly via Emscripten later. |
| **Community** | Largest body of NES reverse-engineering references are in C/C++. The SMB disassembly community speaks this language. |

### Why NOT Godot/Unity/Rust
- **Godot/Unity**: Engine abstractions fight pixel-perfect NES rendering. Tilemaps, physics, and cameras all have opinions that don't match NES behavior. More time fighting the engine than building the game.
- **Rust**: SDL2 bindings exist but are less mature. Borrow checker adds friction for game entity systems. Smaller game dev community for reference.

### Build System
- **CMake** for cross-platform builds
- **vcpkg** or system packages for SDL2/SDL2_image/SDL2_mixer dependencies
- C++17 standard (structured bindings, std::optional, constexpr if)

---

## NES Technical Reference

### Display
- **Resolution**: 256×240 pixels (we render to this, then scale to window)
- **Tile size**: 16×16 pixels for world tiles (the NES uses 8×8 tiles internally but SMB treats them as 16×16 metatiles)
- **Sprite size**: 8×16 pixels per sprite tile; Mario is assembled from multiple sprite tiles
- **Frame rate**: 60 FPS (NTSC). Our game loop targets exactly 60 FPS with fixed timestep.
- **Aspect ratio**: 8:7 pixel aspect ratio (NES pixels were slightly taller than wide). We'll render at 256×240 and optionally stretch to 292×240 for authentic CRT look.

### Color Palette
The NES has a fixed palette of 54 usable colors. SMB uses specific subsets:
- **Sky**: #5C94FC (light blue)
- **Ground/Brick brown**: #D07030, #E09060
- **Mario red**: #B81810 (hat/shirt), #F83800 (lighter red)
- **Mario skin**: #F0D0B0
- **Mario overalls**: #6844FC (actually blue-purple on NES)
- **Green (Luigi/pipes)**: #00A800, #44A800
- **Black outlines**: #000000
- **White highlights**: #FCFCFC
- **? Block gold**: #F8B800, #F0A000

### The Color Palette System
Each 16×16 metatile is assigned one of 4 background palettes (each palette = 3 colors + shared background color). Sprites use 4 separate sprite palettes. This constrains the visual design and we should honor it for authenticity.

---

## Mario Physics Model (Critical)

The original SMB physics are what make the game *feel* right. These values come from
reverse-engineering the NES ROM and must be replicated precisely using fixed-point math.

### Coordinate System
- Internal positions use **sub-pixel precision**: 1 pixel = 256 sub-pixels (16.8 fixed point on NES, we'll use float or fixed-point)
- Velocities are in **pixels per frame** (at 60fps)
- The game world is measured in 16×16 tiles

### Horizontal Movement

| State | Acceleration | Max Speed | Deceleration (friction) |
|---|---|---|---|
| Walking (no B held) | 0.00390625 px/f² | 1.5625 px/f | 0.0 px/f² (instant stop concept - actually uses skid decel) |
| Running (B held) | 0.00390625 px/f² | 2.5 px/f | 0.0 px/f² |
| Skidding (turning around) | 0.00390625 px/f² (opposing direction) | — | — |

Key behaviors:
- Mario has **momentum** — he doesn't stop instantly, he decelerates
- **Skidding** animation plays when the player reverses direction while Mario has velocity
- Air control exists but is reduced (Mario can adjust horizontal velocity mid-jump but with lower acceleration)
- Minimum walk speed threshold: below this, Mario snaps to zero

### Vertical Movement / Jumping

This is the single most important system to get right.

| Parameter | Value |
|---|---|
| **Initial jump velocity (walking)** | -4.0 px/f |
| **Initial jump velocity (running)** | -5.0 px/f |
| **Gravity (A held)** | 0.2 px/f² |
| **Gravity (A released)** | 0.7 px/f² |
| **Terminal velocity** | 4.75 px/f |
| **Jump height (tap, walking)** | ~32 pixels (2 tiles) |
| **Jump height (hold, walking)** | ~64 pixels (4 tiles) |
| **Jump height (hold, running)** | ~80 pixels (5 tiles) |

**Variable-height jumping**: When the player holds the jump button, low gravity applies. The instant they release it, high gravity kicks in, cutting the jump short. This is the core of Mario's feel.

**Jump buffering**: The original game allows pressing jump a few frames before landing and it still registers. We should implement this.

**Coyote time**: NOT in the original (Mario falls immediately when walking off edges). We should match this — no coyote time.

### Swimming Physics (World 2-2, future)
Different gravity, button press gives upward impulse. Not needed for World 1-1.

---

## Collision System

### AABB (Axis-Aligned Bounding Box)
All collision in SMB is rectangle-based. No pixel-perfect collision.

### Mario's Collision Box
- **Small Mario**: 12×16 pixels (slightly narrower than the 16px sprite)
- **Big Mario**: 12×32 pixels
- The collision box is horizontally centered on the sprite
- Collision box is a couple pixels narrower than visual — this is intentional and makes the game feel fair

### Tile Collision
- The world is a grid of 16×16 tiles
- Mario checks collision against tiles he overlaps
- **Top collision** (hitting blocks from below): triggers ? block/brick response
- **Bottom collision** (landing): Mario lands, sets grounded state
- **Side collision**: Mario stops horizontal movement
- Collision is checked horizontally and vertically separately (resolve one axis at a time)

### Entity Collision
- **Stomp detection**: Mario's bottom edge overlaps enemy's top half → stomp kill
- **Side/bottom hit**: Mario takes damage
- **Power-up pickup**: Overlap with power-up entity → collect
- **Coin pickup**: Overlap with coin → collect
- **Fireball hit**: Fireball overlaps enemy → kill

### One-Way Platforms
Some platforms in later levels are one-way (pass through from below). World 1-1 doesn't have these but the system should support it.

---

## Sprite & Animation System

### Sprite Sheet Organization
All game graphics stored in sprite sheets (PNG files with transparency):
- `mario_sprites.png` — All Mario states/frames
- `enemies.png` — Goomba, Koopa, Piranha Plant
- `tiles.png` — World tileset (ground, bricks, ? blocks, pipes, clouds, bushes)
- `items.png` — Mushroom, fire flower, star, coin, fireball
- `hud.png` — Numbers, letters, icons for the HUD

### Mario Animation States & Frame Counts

| State | Frames | Frame Duration | Notes |
|---|---|---|---|
| Idle | 1 | — | Standing still |
| Walk | 3 | Speed-dependent (faster walk = faster animation) | Cycles through 3 frames |
| Run | 3 | Faster cycle than walk | Same frames, faster timing |
| Skid | 1 | — | Turning around while moving |
| Jump | 1 | — | Single airborne pose |
| Fall | 1 | — | Same as jump in original |
| Climb (flagpole) | 2 | 8 frames each | Alternating hand-over-hand |
| Die | 1 | — | Hands up, bounces off screen |
| Swim | 3 | — | Future (not in 1-1) |
| Crouch | 1 | — | Big Mario only |
| Fire throw | 1 | 4 frames | Briefly overlays on walk/run |

Each state exists in 3 variants:
1. **Small Mario** (16×16)
2. **Big Mario** (16×32)
3. **Fire Mario** (16×32, different palette)

### Animation Timing
Walk animation speed scales inversely with Mario's horizontal velocity:
- Slow walk: ~8 frames per animation frame
- Fast walk: ~4 frames per animation frame
- Running: ~3 frames per animation frame

### Enemy Animations

| Enemy | Frames | Duration | Notes |
|---|---|---|---|
| Goomba walk | 2 | 8 frames each | Alternating feet |
| Goomba squished | 1 | 30 frames then despawn | Flat sprite |
| Koopa walk | 2 | 8 frames each | Alternating legs |
| Koopa shell (idle) | 1 | — | Stationary shell |
| Koopa shell (moving) | 4 | 2 frames each | Spinning fast |
| Piranha Plant | 2 | 8 frames each | Mouth open/closed while emerging |

### Sprite Flipping
Mario's sprite is horizontally flipped based on facing direction (saves sprite sheet space — only need to draw right-facing sprites).

---

## Tile & World System

### Tilemap Architecture
The world is a 2D grid of 16×16 tiles. World 1-1 is:
- **Width**: 211 tiles (3376 pixels)
- **Height**: 15 tiles (240 pixels — fills the screen vertically)
- The bottom 2 rows are ground (unless there's a pit)
- The top 2 rows are reserved for HUD

### Tile Types

| Tile ID | Type | Properties |
|---|---|---|
| 0 | Empty/Sky | No collision, transparent |
| 1 | Ground | Solid, top/side/bottom collision |
| 2 | Brick | Solid, breakable (big Mario), bounces (small Mario), may contain coins |
| 3 | ? Block (coin) | Solid, dispenses coin when hit from below, becomes empty block |
| 4 | ? Block (mushroom) | Solid, dispenses mushroom/fire flower, becomes empty block |
| 5 | ? Block (star) | Solid, dispenses star, becomes empty block |
| 6 | ? Block (1UP) | Solid, dispenses 1UP mushroom, becomes empty block |
| 7 | Empty Block | Solid, used/spent ? block appearance |
| 8 | Hidden Block | No collision (pass through), becomes visible when hit from below |
| 9 | Pipe Top-Left | Solid, visual |
| 10 | Pipe Top-Right | Solid, visual |
| 11 | Pipe Body-Left | Solid, visual |
| 12 | Pipe Body-Right | Solid, visual |
| 13 | Flagpole | Special interaction (end of level) |
| 14 | Castle tile | Solid, visual (end castle) |
| 15+ | Decorative | Cloud, bush, hill (no collision, background layer) |

### Scrolling Camera
- Camera only scrolls **right** (original SMB does not scroll left!)
- Camera is locked to Mario's position with a dead zone
- Camera X = max(camera X, Mario X - dead_zone_offset)
- Camera never scrolls past the level boundaries

### Background Layer
Clouds, hills, and bushes are rendered behind the tile layer. In the original NES game, clouds and bushes are actually the same sprite with different palettes. We should replicate this detail.

### Level Data Format
Levels stored as JSON or binary files:
```json
{
  "width": 211,
  "height": 15,
  "time_limit": 400,
  "background_color": "#5C94FC",
  "tilemap": [ [0,0,0,...], [0,0,0,...], ... ],
  "entities": [
    {"type": "goomba", "x": 22, "y": 12},
    {"type": "goomba", "x": 40, "y": 12},
    {"type": "koopa", "x": 107, "y": 12}
  ],
  "pipes": [
    {"x": 28, "y": 10, "height": 3, "enterable": false},
    {"x": 38, "y": 9, "height": 4, "enterable": true, "destination": "1-2"}
  ]
}
```

---

## World 1-1 Complete Layout Reference

The level proceeds left-to-right (all positions in tile coordinates, origin top-left):

### Segment 1: Starting Area (tiles 0-20)
- Full ground floor (rows 13-14)
- ? block at (16, 9) — coin
- Brick-?-Brick-?-Brick cluster at (20-24, 9) — center ? has mushroom
- ? block at (22, 5) — coin (high block)

### Segment 2: Pipes & Goombas (tiles 25-50)
- Goomba at tile 22
- Pipe (height 2) at tiles 28-29
- Pipe (height 3) at tiles 38-39
- Pipe (height 4) at tiles 46-47 (this one leads to underground bonus in full game)
- Pipe (height 4) at tiles 57-58
- Goombas scattered between pipes

### Segment 3: Block Clusters (tiles 60-90)
- Brick-?-Brick cluster around tile 78
- ? block cluster with star
- Hidden 1UP block
- Goombas in pairs

### Segment 4: Elevated Platforms (tiles 90-120)
- Brick row at height 5 (8 bricks, one contains star)
- Gap/pit (first pit in the game, ~2 tiles wide)
- Elevated brick platforms
- More goombas

### Segment 5: Staircases & Koopa (tiles 130-170)
- First staircase (ascending right, 4 blocks high)
- Gap after staircase
- Descending staircase
- Koopa Troopa on flat area
- Brick row with coins
- Pair of goombas

### Segment 6: Final Run (tiles 170-211)
- Ascending staircase (8 blocks high)
- Flagpole at tile ~198
- Flat ground to castle
- Castle at far right

**Key enemy placements (tile X coordinates, approximate)**:
- Goomba: 22, 40, 51, 52, 80, 82, 97, 99, 114, 116, 124, 126, 170, 172
- Koopa Troopa: 107
- Piranha Plants: In tall pipes

---

## Entity System

### Entity Base Class
All game objects inherit from a common Entity:
```
Entity:
  - position (x, y) as float (sub-pixel precision)
  - velocity (vx, vy)
  - acceleration (ax, ay)
  - bounding_box (offset from position + width/height)
  - sprite/animation state
  - active flag (for activation distance from camera)
  - facing direction
  - alive/dead state
```

### Entity Activation
Enemies and items are **not active** until the camera scrolls close to them.
- Activation distance: ~1 tile ahead of the right screen edge
- Once activated, entities stay active even if they scroll off-screen left
- Entities that fall below the world are destroyed

### Mario Entity (Special)
Mario has additional state:
- `power_state`: SMALL, BIG, FIRE
- `invincibility_timer`: After taking damage or getting star
- `star_timer`: Star power countdown
- `is_grounded`: On solid surface
- `is_jumping`: In jump arc
- `is_crouching`: Ducking (big Mario)
- `is_dead`: Death animation playing
- `lives`: Life count
- `coins`: Coin count
- `score`: Score value

### Power-Up State Machine
```
SMALL --[mushroom]--> BIG --[fire flower]--> FIRE
FIRE  --[damage]----> BIG --[damage]-------> SMALL --[damage]--> DEAD
                                              (with invincibility frames)
```
Transition from SMALL→BIG plays growth animation (Mario flickers between sizes).
Transition from BIG/FIRE→SMALL plays shrink animation with invincibility frames.

---

## Enemy Behavior Details

### Goomba
- Spawns facing left, walks left at constant speed (0.5 px/frame)
- Affected by gravity (falls off ledges)
- Turns around when hitting a wall
- **Stomped**: Plays squished sprite for ~30 frames then despawns, Mario bounces up
- **Fireball**: Dies immediately, flips upside down and falls off screen
- **Shell/Star hit**: Same as fireball
- **Falls in pit**: Despawns

### Koopa Troopa (Green)
- Walks left at constant speed (0.5 px/frame)
- Falls off ledges (green variant; red variant turns around at edges — not in 1-1)
- **Stomped**: Retreats into shell, stops moving
  - Shell can be kicked by walking into it (slides in that direction)
  - Moving shell kills all enemies it contacts
  - Moving shell bounces off walls
  - Moving shell can kill Mario if it hits him from the side
  - Shell can be stomped again to stop it
  - Shell speed: ~3.0 px/frame
- **Fireball**: Shell flies off screen (flipped)

### Piranha Plant
- Lives inside pipes
- Emerges upward over ~48 frames, pauses at top for ~48 frames, retreats over ~48 frames
- **Will NOT emerge if Mario is standing adjacent to or on top of the pipe**
- Cannot be stomped (Mario takes damage if touching)
- Killed by fireball or star

---

## Power-Up System

### Super Mushroom
- Spawns from ? block, rises up out of block over ~16 frames
- Once fully emerged: moves right at 1.5 px/frame
- Affected by gravity
- Bounces off walls (reverses direction)
- Falls off ledges
- Collected on contact with Mario → triggers growth

### Fire Flower
- Spawns from ? block (only when Mario is already BIG or FIRE)
- If Mario is SMALL, the ? block gives a mushroom instead
- Fire flower does NOT move — stays on the block
- Collected on contact → Mario becomes FIRE

### Starman
- Spawns from ? block or hidden block
- Bounces continuously (hops in a sine-like pattern)
- Moves right, bounces off walls
- Falls off ledges (but keeps bouncing)
- Collected on contact → 10 seconds of invincibility
- During star power: enemies die on contact, Mario palette cycles rapidly

### 1-UP Mushroom
- Identical movement to Super Mushroom
- Green instead of red
- Collected → +1 life

### Coins
- **Static coins**: Float in the air at fixed positions, collected on contact
- **Block coins**: Appear when hitting ? block, animate upward and despawn
- **100 coins = 1 extra life**

---

## Scoring System

| Action | Points |
|---|---|
| Stomp enemy | 100 (increases with consecutive stomps: 100, 200, 400, 800, 1000, 2000, 4000, 8000, 1UP) |
| Fireball kill | 200 |
| Shell kill | 500 per enemy |
| Kick shell | 400 |
| Coin | 200 |
| Mushroom | 1000 |
| Fire Flower | 1000 |
| Star | 1000 |
| 1-UP | No points (gives life) |
| Flagpole | 100-5000 depending on height |
| Time bonus | Remaining time × 50 |

### Consecutive Stomp Combo
When Mario stomps multiple enemies without touching the ground, each successive stomp gives more points: 100 → 200 → 400 → 800 → 1000 → 2000 → 4000 → 8000 → 1UP.

---

## HUD (Heads-Up Display)

Rendered in the top 32 pixels of the screen (rows 0-1 of tiles):

```
MARIO                    WORLD  TIME
000000  ×00  ×00  ×00    1-1    400
(score) (coins)(lives)  (level)(timer)
```

- **Score**: 6 digits, left-padded with zeros
- **Coin counter**: Coin icon + ×XX (resets at 100, gives 1UP)
- **World indicator**: Current world-level
- **Timer**: Counts down from 400, at ~2.5 per real second. When timer hits 0, Mario dies.

The HUD font is the NES system font — blocky 8×8 pixel characters.

---

## Game State Machine

```
                    ┌──────────┐
                    │  BOOT    │
                    └────┬─────┘
                         │
                    ┌────▼─────┐
              ┌─────│  TITLE   │─────┐
              │     └────┬─────┘     │
              │          │ Start     │
              │     ┌────▼─────┐     │
              │     │LEVEL_LOAD│     │
              │     └────┬─────┘     │
              │          │           │
              │     ┌────▼─────┐     │
              │     │ PLAYING  │◄──┐ │
              │     └──┬──┬──┬─┘   │ │
              │        │  │  │     │ │
              │   Pause│  │  │Die  │ │
              │   ┌────▼┐ │ ┌▼───┐ │ │
              │   │PAUSE│ │ │DEAD│ │ │
              │   └────┬┘ │ └──┬─┘ │ │
              │   Resume│ │    │   │ │
              │        │  │    │Lives>0
              │        └──┘    │   │ │
              │           Complete │ │
              │           ┌────▼─┐ │ │
              │           │CLEAR │─┘ │
              │           └──────┘   │
              │                Lives=0│
              │           ┌────────┐ │
              └───────────│GAMEOVER│─┘
                          └────────┘
```

### State Transitions
- **TITLE**: Press Start → LEVEL_LOAD
- **LEVEL_LOAD**: Brief "WORLD 1-1" screen with life count, then → PLAYING
- **PLAYING**: Normal gameplay
- **PAUSE**: Freezes game, resumes on button press
- **DEAD**: Death animation (Mario pops up, falls off screen), then:
  - Lives > 0 → LEVEL_LOAD (restart level)
  - Lives = 0 → GAMEOVER
- **CLEAR**: Flagpole → walk to castle → fireworks → score tally → next level
- **GAMEOVER**: "GAME OVER" text, then → TITLE

---

## Audio System

### Sound Effects (Priority-based)
The NES has limited audio channels. Sound effects have priority and can interrupt each other.

| Sound | Trigger | Priority |
|---|---|---|
| Jump | Mario leaves ground | Medium |
| Stomp | Enemy stomped | High |
| Coin | Coin collected | Medium |
| Bump | Hit block from below (empty/brick) | Low |
| Power-up appear | Mushroom/star emerges | Medium |
| Power-up collect | Mario touches mushroom | High |
| Grow | Mario grows (SMALL→BIG) | High |
| Shrink | Mario shrinks (damage) | High |
| Fireball | Mario throws fireball | Medium |
| Kill | Enemy killed by fireball/shell/star | Medium |
| Pipe | Mario enters pipe | High |
| Death | Mario dies | Highest |
| Flagpole | Reaching flagpole | Highest |
| 1-UP | Extra life | High |
| Time warning | Timer < 100 | Highest (speeds up music) |

### Music
- **Overworld theme**: The iconic SMB theme (loops during gameplay)
- **Underground theme**: For underground bonus areas
- **Star power theme**: Plays during invincibility (overrides normal music)
- **Death jingle**: Short, plays on death
- **Level clear fanfare**: Plays at flagpole
- **Game over jingle**: Short, plays on game over
- **Hurry up**: Overworld theme at faster tempo when timer < 100

We'll use OGG/WAV files created to match the NES square wave / triangle wave / noise channel sounds. Consider using a chiptune synthesizer to generate authentic sounds.

---

## Phased Implementation Plan

### Phase 1: Core Engine Foundation

**Goal**: Window opens, renders at NES resolution, processes input, runs at 60fps.

**Tasks**:
1. Set up CMake build system with SDL2 dependencies
2. Create `Window` class — opens an SDL window, creates renderer
3. Create `Renderer` class — renders to 256×240 texture, scales to window
4. Create `Input` class — polls keyboard/gamepad, maps to NES-style buttons (D-pad, A, B, Start, Select)
5. Create `Game` class — fixed-timestep game loop (16.67ms per frame)
6. Create `Timer` class — tracks delta time, frame counting
7. Render a solid blue (#5C94FC) screen to verify pipeline works
8. Verify input polling with debug output
9. Create `ResourceManager` — loads textures from PNG files

**Deliverable**: Blue window at 768×720 (3× NES resolution), steady 60fps, keyboard input captured.

---

### Phase 2: Sprite System & Mario Rendering

**Goal**: Mario sprite renders on screen with all animation states.

**Tasks**:
1. Create pixel art sprite sheets for Mario (small, big, fire — all states)
   - Each sprite hand-pixeled at native NES resolution
   - Match original game's color palette exactly
   - Small Mario: 16×16 per frame
   - Big Mario: 16×32 per frame
2. Create `SpriteSheet` class — loads texture, defines frame regions
3. Create `Sprite` class — references a region of a sprite sheet, can be flipped
4. Create `Animation` class — sequences of sprites with frame durations
5. Define all Mario animations (idle, walk×3, jump, skid, crouch, climb, die, fire-throw)
6. Render Mario at center-bottom of screen, cycle through animations with keyboard input
7. Implement facing direction (sprite flipping)

**Deliverable**: Mario renders in all states, animations play correctly, pixel-perfect NES look.

---

### Phase 3: Mario Physics & Movement

**Goal**: Mario moves with authentic SMB physics.

**Tasks**:
1. Implement fixed-point or high-precision position tracking (sub-pixel)
2. Implement horizontal acceleration / deceleration / max speed
3. Implement running (B button held increases max speed)
4. Implement skidding (direction reversal while moving)
5. Implement jumping with variable height (A button hold duration)
6. Implement dual gravity (low while A held, high when released)
7. Implement terminal velocity
8. Implement jump buffering (press A slightly before landing)
9. Tune all values to match original game feel
10. Animation state machine drives which animation plays based on physics state
11. Walk animation speed scales with horizontal velocity

**Deliverable**: Mario runs, jumps, and skids exactly like the original. No world collision yet — he flies around freely for testing.

---

### Phase 4: Tilemap & World Rendering

**Goal**: World 1-1 renders correctly with scrolling camera.

**Tasks**:
1. Create pixel art tileset matching original:
   - Ground/floor tiles (brown)
   - Sky (solid color)
   - Brick blocks
   - ? blocks (animated — 3 frame shimmer)
   - Empty/used blocks
   - Pipe tiles (top-left, top-right, body-left, body-right)
   - Flagpole segments
   - Castle tiles
   - Decorative: clouds, hills, bushes
2. Create `Tile` data structure (type, solidity, animation state)
3. Create `Tilemap` class — 2D grid of tiles, renders visible portion
4. Create `Camera` class — follows Mario, only scrolls right, clamps to level bounds
5. Create `Level` class — loads level data from file
6. Build World 1-1 level data file with accurate tile placement
7. Implement background decoration layer (clouds, hills, bushes behind tiles)
8. Implement ? block shimmer animation (global timer, all ? blocks animate in sync)

**Deliverable**: World 1-1 renders beautifully. Camera scrolls right as Mario moves. All tiles visually match the original. No collision yet.

---

### Phase 5: Collision System

**Goal**: Mario collides with the world and interacts with blocks.

**Tasks**:
1. Create `AABB` struct (position + size)
2. Implement tile collision detection:
   - Check which tiles Mario's bounding box overlaps
   - Resolve horizontal collision (push out of walls)
   - Resolve vertical collision (land on ground, hit ceiling)
   - Process each axis independently
3. Implement block hit detection (Mario hits block from below):
   - ? block: trigger content spawn, change to empty block, play bump animation
   - Brick: bounce animation if small Mario, break if big Mario
   - Hidden block: reveal and trigger content
4. Implement brick break effect (particle animation — 4 chunks fly outward)
5. Implement block bump animation (block rises ~4px and returns)
6. Implement pit detection (Mario falls below screen → death)
7. Implement pipe collision (solid objects)

**Deliverable**: Mario walks on ground, bumps into walls, hits blocks from below, falls into pits. World is physically solid.

---

### Phase 6: Items & Power-Up System

**Goal**: Blocks spawn items, Mario collects power-ups, power states work.

**Tasks**:
1. Create pixel art for items (mushroom, fire flower, star, 1UP, coin)
2. Implement `Mushroom` entity:
   - Rise-out-of-block animation
   - Move right, affected by gravity, bounce off walls, fall off ledges
   - Collected on Mario contact → grow Mario
3. Implement `FireFlower` entity:
   - Rise-out-of-block animation
   - Stationary on block
   - Only spawns if Mario is BIG (otherwise spawn mushroom)
   - Collected → Mario becomes FIRE
4. Implement `Starman` entity:
   - Rise-out-of-block, then bouncing movement
   - Collected → invincibility timer (10 seconds)
5. Implement `Coin` entity:
   - Static coins in air (collect on overlap)
   - Block coins (animate upward and despawn)
6. Implement Mario power state transitions:
   - SMALL → BIG (growth animation with flicker)
   - BIG → FIRE (palette swap animation)
   - Damage: FIRE → BIG or BIG → SMALL (shrink animation + invincibility frames)
   - SMALL + damage → DEAD
7. Implement invincibility frames (Mario flickers, ignores damage for ~2 seconds)
8. Implement star power (palette cycling, enemies die on contact, timer countdown)
9. Implement coin counter (100 coins = 1UP)

**Deliverable**: Full power-up system works. Mario grows, shrinks, throws fireballs, gets stars. Items behave authentically.

---

### Phase 7: Fireballs

**Goal**: Fire Mario can throw fireballs.

**Tasks**:
1. Create fireball sprite (animated, 4 frames rotating)
2. Implement fireball entity:
   - Spawns from Mario's position when B is pressed (Fire state only)
   - Max 2 fireballs on screen at once
   - Moves in facing direction at ~3.5 px/frame
   - Affected by gravity, bounces off ground (bounces at fixed height)
   - Bounces off walls → despawns (or reverses in some implementations)
   - Dies on enemy contact → enemy dies
   - Despawns after leaving screen or hitting a wall
3. Fireball-enemy collision detection
4. Fireball throw animation on Mario (brief arm-forward pose)

**Deliverable**: Fire Mario throws bouncing fireballs that kill enemies.

---

### Phase 8: Enemy System

**Goal**: All World 1-1 enemies spawn and behave correctly.

**Tasks**:
1. Create pixel art for enemies (Goomba, Koopa Troopa, Piranha Plant)
2. Implement entity activation system (spawn when camera approaches)
3. Implement `Goomba`:
   - Walk left at constant speed
   - Fall off ledges (affected by gravity)
   - Turn around on wall collision
   - Stomp: squished sprite → despawn
   - Fireball/shell/star: flip upside-down, fall off screen
4. Implement `KoopaTroopa`:
   - Walk left, fall off ledges
   - Stomp → shell state (stopped)
   - Kick shell → shell moves fast, kills enemies, bounces off walls
   - Shell-Mario collision → damage (if shell is moving)
   - Re-stomp moving shell → stops it
5. Implement `PiranhaPlant`:
   - Emerge/retract cycle from pipe
   - Pause when Mario is adjacent to pipe
   - Cannot be stomped, killed by fireball/star only
6. Mario-enemy collision:
   - Top of enemy → stomp (Mario bounces, enemy dies/shells)
   - Side/bottom of enemy → Mario takes damage
7. Implement consecutive stomp bonus (points increase without touching ground)

**Deliverable**: Goombas walk around and die when stomped. Koopa shells slide and kill other enemies. Piranha Plants pop in/out of pipes. Full enemy ecosystem works.

---

### Phase 9: HUD, Score, & Game States

**Goal**: Complete game state machine with HUD and scoring.

**Tasks**:
1. Create pixel font for HUD (NES-style 8×8 characters)
2. Implement HUD rendering:
   - Score display (top-left)
   - Coin counter with coin icon
   - World indicator
   - Timer countdown
3. Implement scoring for all actions (see scoring table above)
4. Implement floating score text (pops up when earning points, floats up and fades)
5. Implement timer system:
   - Starts at 400, counts down at ~2.5/second
   - Timer < 100: music speeds up
   - Timer = 0: Mario dies
6. Implement game state machine:
   - Title screen (SUPER MARIO BROS logo, press start prompt)
   - Level intro screen (WORLD 1-1, Mario ×3)
   - Playing state
   - Pause state (freezes everything)
   - Death state (death animation, life lost)
   - Game over state
7. Implement lives system (start with 3 lives)
8. Implement level restart on death
9. Implement game over → title screen flow

**Deliverable**: Full game loop from title screen through gameplay with HUD, scoring, lives, and game over.

---

### Phase 10: Level Completion & Flagpole

**Goal**: Mario can complete World 1-1.

**Tasks**:
1. Implement flagpole entity:
   - Mario touches flagpole → slide down animation
   - Flag slides down with Mario
   - Points awarded based on height Mario touched (higher = more points)
   - Points: 100 (bottom) → 2000 → 4000 → 5000 (top)
2. Implement post-flagpole sequence:
   - Mario walks right to castle entrance
   - Flag raises on castle
   - Timer counts down → remaining time × 50 points
   - Fireworks (if last digit of timer is 1, 3, or 6 → that many fireworks)
   - "Level clear" fanfare
3. Implement castle entrance animation (Mario walks in, disappears)
4. Implement level transition (in full game, would go to 1-2)

**Deliverable**: Mario can complete the level with full end sequence, scoring, and celebration.

---

### Phase 11: Audio

**Goal**: Authentic sound effects and music.

**Tasks**:
1. Create or source NES-authentic sound effects:
   - Generate using chiptune tools (FamiTracker, BFXR, or similar)
   - Jump, stomp, coin, bump, powerup, grow, shrink, fireball, kill, death, pipe, 1up, flagpole, time warning
2. Create or source NES-authentic music:
   - Overworld theme (the iconic tune)
   - Star power theme
   - Death jingle
   - Level clear fanfare
   - Game over jingle
   - Hurry-up variant (sped up overworld theme)
3. Implement `Audio` class with SDL2_mixer:
   - Music channel (one at a time, loops)
   - Sound effect channels (multiple simultaneous)
   - Priority system (important sounds override less important ones)
4. Hook audio triggers into game events:
   - Jump → jump SFX
   - Coin → coin SFX
   - Stomp → stomp SFX
   - etc.
5. Implement music tempo change when timer < 100

**Deliverable**: Game sounds and feels like the original. Iconic theme plays during gameplay.

---

### Phase 12: Polish & Authenticity Pass

**Goal**: Make it indistinguishable from the original.

**Tasks**:
1. **Screen transitions**: Screen wipe effects between states
2. **Death animation**: Mario pops upward, pauses, then falls off screen with precise timing
3. **Growth animation**: Mario flickers between small/big sprites ~6 times
4. **Star palette cycling**: Mario's palette rotates through colors rapidly
5. **Invincibility flicker**: Mario sprite toggles visible/invisible every 2 frames
6. **Coin spin animation**: ? block coins spin upward (4-frame rotation)
7. **Brick particle effects**: 4 quarter-brick chunks with parabolic trajectories
8. **Flagpole timing**: Precise recreation of the slide-down, walk-to-castle sequence
9. **Pipe warp transition**: If implementing underground bonus room
10. **Screen shake**: None in original (don't add it)
11. **Mario's exact acceleration curves**: Fine-tune to frame-perfect accuracy
12. **Enemy spawn distances**: Match original activation triggers
13. **Score popup float speed and duration**: Match original timing
14. **Cloud/bush visual equivalence**: Clouds and bushes use same shape, different palette (NES trick)

**Deliverable**: A polished, complete World 1-1 that feels exactly like playing the original.

---

## Directory Structure

```
nintendoclone/
├── CMakeLists.txt
├── PLAN.md
├── README.md (auto-generated later)
├── src/
│   ├── main.cpp                    # Entry point
│   ├── core/
│   │   ├── Game.h / Game.cpp       # Game loop, state machine
│   │   ├── Window.h / Window.cpp   # SDL window wrapper
│   │   ├── Renderer.h / Renderer.cpp # NES-res render target + scaling
│   │   ├── Input.h / Input.cpp     # Input polling, button mapping
│   │   ├── Audio.h / Audio.cpp     # Sound/music playback
│   │   ├── Timer.h / Timer.cpp     # Frame timing, fixed timestep
│   │   └── ResourceManager.h / ResourceManager.cpp
│   ├── physics/
│   │   ├── AABB.h                  # Bounding box struct
│   │   └── Collision.h / Collision.cpp # Tile + entity collision
│   ├── entities/
│   │   ├── Entity.h / Entity.cpp   # Base entity class
│   │   ├── Mario.h / Mario.cpp     # Player character
│   │   ├── Goomba.h / Goomba.cpp
│   │   ├── KoopaTroopa.h / KoopaTroopa.cpp
│   │   ├── PiranhaPlant.h / PiranhaPlant.cpp
│   │   ├── Mushroom.h / Mushroom.cpp
│   │   ├── FireFlower.h / FireFlower.cpp
│   │   ├── Starman.h / Starman.cpp
│   │   ├── OneUpMushroom.h / OneUpMushroom.cpp
│   │   ├── Fireball.h / Fireball.cpp
│   │   └── Coin.h / Coin.cpp
│   ├── world/
│   │   ├── Tile.h                  # Tile types, properties
│   │   ├── Tilemap.h / Tilemap.cpp # 2D tile grid, rendering
│   │   ├── Camera.h / Camera.cpp   # Scrolling camera
│   │   ├── Block.h / Block.cpp     # Interactive blocks (?, brick, hidden)
│   │   └── Level.h / Level.cpp     # Level data loading
│   ├── graphics/
│   │   ├── Sprite.h / Sprite.cpp
│   │   ├── Animation.h / Animation.cpp
│   │   └── SpriteSheet.h / SpriteSheet.cpp
│   └── states/
│       ├── GameState.h             # State interface
│       ├── TitleState.h / TitleState.cpp
│       ├── LevelIntroState.h / LevelIntroState.cpp
│       ├── PlayState.h / PlayState.cpp
│       ├── PauseState.h / PauseState.cpp
│       ├── DeathState.h / DeathState.cpp
│       └── GameOverState.h / GameOverState.cpp
├── assets/
│   ├── sprites/
│   │   ├── mario_small.png
│   │   ├── mario_big.png
│   │   ├── mario_fire.png
│   │   ├── enemies.png
│   │   └── items.png
│   ├── tiles/
│   │   ├── overworld_tileset.png
│   │   └── castle_tileset.png
│   ├── sounds/
│   │   ├── jump.wav
│   │   ├── stomp.wav
│   │   ├── coin.wav
│   │   ├── bump.wav
│   │   ├── powerup_appear.wav
│   │   ├── powerup_collect.wav
│   │   ├── grow.wav
│   │   ├── shrink.wav
│   │   ├── fireball.wav
│   │   ├── kill.wav
│   │   ├── death.wav
│   │   ├── pipe.wav
│   │   ├── oneup.wav
│   │   ├── flagpole.wav
│   │   └── time_warning.wav
│   ├── music/
│   │   ├── overworld.ogg
│   │   ├── overworld_fast.ogg
│   │   ├── star.ogg
│   │   ├── death.ogg
│   │   ├── level_clear.ogg
│   │   └── game_over.ogg
│   ├── levels/
│   │   └── world_1_1.json
│   └── fonts/
│       └── nes_font.png
└── tools/                          # (future: level editor)
```

---

## Pixel Art Strategy

We are NOT using placeholder programmer art. Every sprite must be hand-pixeled to match the original game's look.

### Approach
1. Reference the original NES sprite sheets (widely available as community resources)
2. Recreate each sprite pixel-by-pixel using the correct NES color palette
3. Use the exact same sprite dimensions as the original
4. Store as PNG with transparency (magenta or alpha channel for transparent pixels)

### Tools
- **Aseprite** (recommended): Best pixel art editor, supports animation, palette management
- **LibreSprite** (free alternative): Open source Aseprite fork
- **GIMP**: Viable but less convenient for sprite animation work
- **Piskel**: Free browser-based option

### Palette Enforcement
Lock the color palette to the NES colors used by SMB. This prevents accidentally using colors that look "too modern" and break the aesthetic.

---

## Testing Strategy

### Manual Testing Checkpoints
At each phase, verify against video recordings of the original game:
1. Does Mario's jump arc match?
2. Does the walk/run speed feel right?
3. Do enemies behave correctly?
4. Do blocks respond properly?
5. Does the scrolling feel right?
6. Does the level layout match?

### Automated Tests
- Unit tests for physics calculations (acceleration, gravity, collision response)
- Unit tests for collision detection (AABB overlap, tile resolution)
- Unit tests for game state transitions
- Unit tests for scoring calculations

### Frame-by-Frame Comparison
Record gameplay and compare frame-by-frame with original game footage for:
- Jump height accuracy
- Enemy movement speed
- Animation timing
- Scrolling behavior

---

## Future Expansion (Post World 1-1)

Once World 1-1 is complete, the engine supports expansion to:
1. **World 1-2** (underground level — new tileset, new layout)
2. **World 1-3** (athletic level — moving platforms, Cheep-Cheeps)
3. **World 1-4** (castle level — fire bars, Bowser, lava)
4. **Worlds 2-8** (reuse engine, add new enemy types: Hammer Bros, Lakitu, Bullet Bill, Blooper, etc.)
5. **2-player mode** (alternating turns, Luigi)
6. **Warp zones** (pipe warps to later worlds)
7. **Underground bonus rooms** (coin rooms accessed via pipes)

The architecture is designed to make this expansion straightforward — new levels are data, new enemies are new Entity subclasses, new tilesets are new sprite sheets.

---

## Summary: Build Order

| Phase | What You Get |
|---|---|
| 1 | Blue window, 60fps loop, input working |
| 2 | Mario renders on screen with all animations |
| 3 | Mario moves with authentic physics (floating in space) |
| 4 | World 1-1 renders with scrolling camera |
| 5 | Mario walks on ground, hits blocks, falls in pits |
| 6 | Power-ups spawn and work (mushroom, fire flower, star) |
| 7 | Fire Mario throws fireballs |
| 8 | Enemies walk, die, shell physics work |
| 9 | HUD, score, lives, game states, title screen |
| 10 | Flagpole completion sequence |
| 11 | Sound effects and music |
| 12 | Polish pass — make it feel perfect |

Each phase builds on the previous one. Each phase produces a playable/testable result. No phase is wasted work.
