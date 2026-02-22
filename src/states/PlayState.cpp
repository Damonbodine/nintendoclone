#include "states/PlayState.h"
#include "core/Game.h"
#include "core/Constants.h"
#include "physics/Collision.h"
#include <algorithm>
#include <cstdio>

PlayState::PlayState(Game& game) : m_game(game) {}

void PlayState::enter() {
    // Build the level
    m_level.buildWorld1_1();

    // Set up camera
    m_camera.setLevelBounds(m_level.getTilemap().getPixelWidth(),
                            m_level.getTilemap().getPixelHeight());
    m_camera.reset();

    // Set up Mario
    m_mario.x = m_level.getMarioStartX();
    m_mario.y = m_level.getMarioStartY();
    m_mario.active = true;
    m_mario.alive = true;

    // Load assets and set up animations
    loadAssets();
    setupMarioAnimations();

    // Spawn entities from level data
    spawnEnemies();

    // Init timer
    m_timer = m_level.getTimeLimit();
    m_timerCounter = 0;
}

void PlayState::exit() {
    // Cleanup
    m_goombas.clear();
    m_koopas.clear();
    m_piranhas.clear();
    m_mushrooms.clear();
    m_fireFlowers.clear();
    m_stars.clear();
    m_fireballs.clear();
    m_coins.clear();
}

void PlayState::loadAssets() {
    auto* renderer = m_game.getRenderer().getSDLRenderer();
    auto& resources = m_game.getResources();

    // Try to load textures — if they don't exist yet, we'll use colored rectangles
    // as placeholder rendering until the pixel art is created.
    SDL_Texture* tilesetTex = resources.loadTexture("assets/tiles/overworld_tileset.png");
    if (tilesetTex) {
        m_tilesetSheet.init(tilesetTex, Constants::TILE_SIZE, Constants::TILE_SIZE);
    }

    SDL_Texture* marioSmallTex = resources.loadTexture("assets/sprites/mario_small.png");
    if (marioSmallTex) {
        m_marioSmallSheet.init(marioSmallTex, 16, 16);
    }

    SDL_Texture* marioBigTex = resources.loadTexture("assets/sprites/mario_big.png");
    if (marioBigTex) {
        m_marioBigSheet.init(marioBigTex, 16, 32);
    }

    SDL_Texture* enemyTex = resources.loadTexture("assets/sprites/enemies.png");
    if (enemyTex) {
        m_enemySheet.init(enemyTex, 16, 16);
    }

    SDL_Texture* itemTex = resources.loadTexture("assets/sprites/items.png");
    if (itemTex) {
        m_itemSheet.init(itemTex, 16, 16);
    }

    // Load sounds
    auto& audio = m_game.getAudio();
    audio.loadSound("jump", "assets/sounds/jump.wav");
    audio.loadSound("stomp", "assets/sounds/stomp.wav");
    audio.loadSound("coin", "assets/sounds/coin.wav");
    audio.loadSound("bump", "assets/sounds/bump.wav");
    audio.loadSound("powerup", "assets/sounds/powerup_collect.wav");
    audio.loadSound("death", "assets/sounds/death.wav");
    audio.loadMusic("overworld", "assets/music/overworld.ogg");
}

void PlayState::setupMarioAnimations() {
    // Set up Mario's animations.
    // These reference frames in the sprite sheet.
    // Small Mario: 16x16 per frame, laid out horizontally
    // Frame 0: idle
    // Frame 1-3: walk
    // Frame 4: jump
    // Frame 5: skid
    // Frame 6: die

    if (m_marioSmallSheet.getTexture()) {
        Animation idle;
        idle.init(&m_marioSmallSheet, { {0, 1} });

        Animation walk;
        walk.init(&m_marioSmallSheet, { {1, 6}, {2, 6}, {3, 6} });

        Animation jump;
        jump.init(&m_marioSmallSheet, { {4, 1} });

        Animation skid;
        skid.init(&m_marioSmallSheet, { {5, 1} });

        Animation die;
        die.init(&m_marioSmallSheet, { {6, 1} }, false);

        m_mario.sprite.addAnimation("idle", idle);
        m_mario.sprite.addAnimation("walk", walk);
        m_mario.sprite.addAnimation("jump", jump);
        m_mario.sprite.addAnimation("skid", skid);
        m_mario.sprite.addAnimation("die", die);

        m_mario.sprite.setAnimation("idle");
    }
}

void PlayState::spawnEnemies() {
    for (const auto& spawn : m_level.getEnemySpawns()) {
        switch (spawn.type) {
            case Level::EnemySpawn::GOOMBA: {
                auto goomba = std::make_unique<Goomba>();
                goomba->x = spawn.x;
                goomba->y = spawn.y;
                goomba->spawnX = spawn.x;
                goomba->spawnY = spawn.y;
                goomba->active = false;  // Activated when camera approaches
                m_goombas.push_back(std::move(goomba));
                break;
            }
            case Level::EnemySpawn::KOOPA: {
                auto koopa = std::make_unique<KoopaTroopa>();
                koopa->x = spawn.x;
                koopa->y = spawn.y;
                koopa->spawnX = spawn.x;
                koopa->spawnY = spawn.y;
                koopa->active = false;
                m_koopas.push_back(std::move(koopa));
                break;
            }
            case Level::EnemySpawn::PIRANHA: {
                auto piranha = std::make_unique<PiranhaPlant>();
                piranha->setPipeTop(spawn.x, spawn.y);
                piranha->spawnX = spawn.x;
                piranha->spawnY = spawn.y;
                piranha->active = false;
                m_piranhas.push_back(std::move(piranha));
                break;
            }
        }
    }
}

void PlayState::update(Game& game) {
    if (m_paused) {
        if (game.getInput().isPressed(Input::START)) {
            m_paused = false;
        }
        return;
    }

    // Pause
    if (game.getInput().isPressed(Input::START)) {
        m_paused = true;
        return;
    }

    if (m_levelComplete) {
        // Handle level complete sequence
        m_flagpoleTimer++;
        return;
    }

    // Update tilemap animations (? block shimmer, bumps)
    m_level.getTilemap().updateAnimations();

    // Activate entities near camera
    activateEntities();

    // Update Mario
    updateMario(game);

    // Update enemies
    updateEnemies();

    // Update items
    updateItems();

    // Collision checks
    // NES quirk: enemy collision only checked on even frames (FrameCounter bit 0 clear)
    Uint64 frameCount = m_game.getTimer().getFrameCount();
    if (!Constants::ENEMY_COLLISION_EVEN_FRAMES_ONLY || (frameCount % 2 == 0)) {
        checkMarioEnemyCollisions();
    }
    checkMarioItemCollisions();
    checkFireballCollisions();
    checkShellCollisions();

    // Update camera
    m_camera.update(m_mario.x, m_mario.y);

    // Update effects
    updateEffects();

    // Update HUD
    updateHUD();
}

void PlayState::updateMario(Game& game) {
    m_mario.handleInput(game.getInput());
    m_mario.update(m_level.getTilemap());

    // Check for block hits from below
    if (m_mario.vy <= 0) {
        // Mario is moving upward — check for block collision above
        AABB marioBox = m_mario.getWorldBounds();
        int leftTile = static_cast<int>(marioBox.left()) / Constants::TILE_SIZE;
        int rightTile = static_cast<int>(marioBox.right() - 0.001f) / Constants::TILE_SIZE;
        int topTile = static_cast<int>(marioBox.top()) / Constants::TILE_SIZE;

        for (int tx = leftTile; tx <= rightTile; tx++) {
            TileType tile = m_level.getTilemap().getTile(tx, topTile);
            if (TileProperties::isInteractive(tile)) {
                handleBlockHit(tx, topTile);
            }
        }
    }
}

void PlayState::updateEnemies() {
    const auto& tilemap = m_level.getTilemap();

    for (auto& goomba : m_goombas) {
        if (goomba->active) {
            goomba->update(tilemap);
        }
    }
    for (auto& koopa : m_koopas) {
        if (koopa->active) {
            koopa->update(tilemap);
        }
    }
    for (auto& piranha : m_piranhas) {
        if (piranha->active) {
            // Check if Mario is near the pipe
            float dist = std::abs(m_mario.x - piranha->spawnX);
            piranha->setMarioNearby(dist < 32);
            piranha->update(tilemap);
        }
    }
}

void PlayState::updateItems() {
    const auto& tilemap = m_level.getTilemap();

    for (auto& mushroom : m_mushrooms) {
        mushroom->update(tilemap);
    }
    for (auto& flower : m_fireFlowers) {
        flower->update(tilemap);
    }
    for (auto& star : m_stars) {
        star->update(tilemap);
    }
    for (auto& fireball : m_fireballs) {
        fireball->update(tilemap);
    }
}

void PlayState::updateEffects() {
    // Update brick break effects
    for (auto& effect : m_brickEffects) {
        effect.update();
    }
    m_brickEffects.erase(
        std::remove_if(m_brickEffects.begin(), m_brickEffects.end(),
            [](const BrickBreakEffect& e) { return !e.active; }),
        m_brickEffects.end());

    // Update coin pop effects
    for (auto& effect : m_coinPopEffects) {
        effect.update();
    }
    m_coinPopEffects.erase(
        std::remove_if(m_coinPopEffects.begin(), m_coinPopEffects.end(),
            [](const CoinPopEffect& e) { return !e.active; }),
        m_coinPopEffects.end());

    // Update score popups
    for (auto& popup : m_scorePopups) {
        popup.y -= 1.0f;
        popup.timer--;
    }
    m_scorePopups.erase(
        std::remove_if(m_scorePopups.begin(), m_scorePopups.end(),
            [](const ScorePopup& p) { return p.timer <= 0; }),
        m_scorePopups.end());
}

void PlayState::updateHUD() {
    // Timer countdown
    m_timerCounter++;
    int framesPerTick = static_cast<int>(Constants::TARGET_FPS / Constants::TIMER_SPEED);
    if (m_timerCounter >= framesPerTick) {
        m_timerCounter = 0;
        m_timer--;

        if (m_timer <= 0) {
            m_timer = 0;
            m_mario.die();
        }
    }
}

void PlayState::checkMarioEnemyCollisions() {
    if (!m_mario.alive || m_mario.isDead()) return;

    AABB marioBox = m_mario.getWorldBounds();

    // Goombas
    for (auto& goomba : m_goombas) {
        if (!goomba->alive || !goomba->active || goomba->isSquished()) continue;

        AABB goombaBox = goomba->getWorldBounds();
        if (!marioBox.overlaps(goombaBox)) continue;

        if (Collision::isStomping(marioBox, goombaBox, m_mario.vy)) {
            // Stomp!
            goomba->stomp();
            m_mario.vy = -4.0f;  // Bounce
            m_mario.stompCombo++;

            int points = 100;  // TODO: scale with stomp combo
            addScore(points);
            addScorePopup(goomba->x, goomba->y, points);
        } else if (!m_mario.isInvincible()) {
            if (m_mario.hasStar()) {
                goomba->killFlip();
                addScore(200);
            } else {
                m_mario.takeDamage();
            }
        }
    }

    // Koopas
    for (auto& koopa : m_koopas) {
        if (!koopa->alive || !koopa->active) continue;

        AABB koopaBox = koopa->getWorldBounds();
        if (!marioBox.overlaps(koopaBox)) continue;

        if (Collision::isStomping(marioBox, koopaBox, m_mario.vy)) {
            koopa->stomp();
            m_mario.vy = -4.0f;

            if (koopa->getKoopaState() == KoopaState::WALKING) {
                addScore(100);
            }
        } else if (koopa->getKoopaState() == KoopaState::SHELL_IDLE) {
            // Kick the shell
            bool kickRight = m_mario.x < koopa->x;
            koopa->kick(kickRight);
            addScore(400);
        } else if (!m_mario.isInvincible()) {
            if (m_mario.hasStar()) {
                koopa->killFlip();
                addScore(200);
            } else {
                m_mario.takeDamage();
            }
        }
    }

    // Piranha Plants
    for (auto& piranha : m_piranhas) {
        if (!piranha->alive || !piranha->active) continue;

        AABB piranhaBox = piranha->getWorldBounds();
        if (!marioBox.overlaps(piranhaBox)) continue;

        if (m_mario.hasStar()) {
            piranha->kill();
            addScore(200);
        } else if (!m_mario.isInvincible()) {
            m_mario.takeDamage();
        }
    }
}

void PlayState::checkMarioItemCollisions() {
    if (!m_mario.alive) return;

    AABB marioBox = m_mario.getWorldBounds();

    // Mushrooms
    for (auto& mushroom : m_mushrooms) {
        if (!mushroom->alive || mushroom->isEmerging()) continue;

        AABB mushroomBox = mushroom->getWorldBounds();
        if (marioBox.overlaps(mushroomBox)) {
            if (mushroom->isOneUp()) {
                m_mario.lives++;
                addScore(0);  // 1-UPs don't give score
            } else {
                m_mario.grow();
                addScore(1000);
            }
            mushroom->alive = false;
            addScorePopup(mushroom->x, mushroom->y, mushroom->isOneUp() ? 0 : 1000);
        }
    }

    // Fire Flowers
    for (auto& flower : m_fireFlowers) {
        if (!flower->alive || flower->isEmerging()) continue;

        AABB flowerBox = flower->getWorldBounds();
        if (marioBox.overlaps(flowerBox)) {
            m_mario.getFire();
            flower->alive = false;
            addScore(1000);
            addScorePopup(flower->x, flower->y, 1000);
        }
    }

    // Stars
    for (auto& star : m_stars) {
        if (!star->alive || star->isEmerging()) continue;

        AABB starBox = star->getWorldBounds();
        if (marioBox.overlaps(starBox)) {
            m_mario.collectStar();
            star->alive = false;
            addScore(1000);
            addScorePopup(star->x, star->y, 1000);
        }
    }

    // Coins
    for (auto& coin : m_coins) {
        if (!coin->alive) continue;

        AABB coinBox = coin->getWorldBounds();
        if (marioBox.overlaps(coinBox)) {
            coin->collect();
            m_mario.coins++;
            if (m_mario.coins >= Constants::COINS_FOR_1UP) {
                m_mario.coins -= Constants::COINS_FOR_1UP;
                m_mario.lives++;
            }
            addScore(200);
        }
    }
}

void PlayState::checkFireballCollisions() {
    for (auto& fireball : m_fireballs) {
        if (!fireball->alive) continue;

        AABB fbBox = fireball->getWorldBounds();

        for (auto& goomba : m_goombas) {
            if (!goomba->alive || !goomba->active || goomba->isSquished()) continue;
            if (fbBox.overlaps(goomba->getWorldBounds())) {
                goomba->killFlip();
                fireball->explode();
                addScore(200);
                addScorePopup(goomba->x, goomba->y, 200);
                break;
            }
        }

        for (auto& koopa : m_koopas) {
            if (!koopa->alive || !koopa->active) continue;
            if (fbBox.overlaps(koopa->getWorldBounds())) {
                koopa->killFlip();
                fireball->explode();
                addScore(200);
                addScorePopup(koopa->x, koopa->y, 200);
                break;
            }
        }

        for (auto& piranha : m_piranhas) {
            if (!piranha->alive || !piranha->active) continue;
            if (fbBox.overlaps(piranha->getWorldBounds())) {
                piranha->kill();
                fireball->explode();
                addScore(200);
                break;
            }
        }
    }
}

void PlayState::checkShellCollisions() {
    for (auto& koopa : m_koopas) {
        if (!koopa->alive || !koopa->isShellMoving()) continue;

        AABB shellBox = koopa->getWorldBounds();

        // Shell vs Goomba
        for (auto& goomba : m_goombas) {
            if (!goomba->alive || !goomba->active || goomba->isSquished()) continue;
            if (shellBox.overlaps(goomba->getWorldBounds())) {
                goomba->killFlip();
                addScore(500);
                addScorePopup(goomba->x, goomba->y, 500);
            }
        }

        // Shell vs other Koopas
        for (auto& otherKoopa : m_koopas) {
            if (otherKoopa.get() == koopa.get()) continue;
            if (!otherKoopa->alive || !otherKoopa->active) continue;
            if (shellBox.overlaps(otherKoopa->getWorldBounds())) {
                otherKoopa->killFlip();
                addScore(500);
                addScorePopup(otherKoopa->x, otherKoopa->y, 500);
            }
        }
    }
}

void PlayState::activateEntities() {
    float activationX = m_camera.getX() + Constants::NES_WIDTH + 16;

    for (auto& goomba : m_goombas) {
        if (!goomba->active && goomba->spawnX < activationX) {
            goomba->active = true;
        }
    }
    for (auto& koopa : m_koopas) {
        if (!koopa->active && koopa->spawnX < activationX) {
            koopa->active = true;
        }
    }
    for (auto& piranha : m_piranhas) {
        if (!piranha->active && piranha->spawnX < activationX) {
            piranha->active = true;
        }
    }
}

void PlayState::handleBlockHit(int tileX, int tileY) {
    bool isBig = m_mario.getPowerState() != PowerState::SMALL;
    auto result = m_level.getTilemap().hitBlockFromBelow(tileX, tileY, isBig);

    if (result.breaksBrick) {
        // Spawn brick break particles
        BrickBreakEffect effect;
        effect.spawn(tileX * Constants::TILE_SIZE, tileY * Constants::TILE_SIZE);
        m_brickEffects.push_back(effect);
    }

    if (result.spawnsItem) {
        float blockWorldX = tileX * Constants::TILE_SIZE;
        float blockWorldY = tileY * Constants::TILE_SIZE;

        switch (result.originalType) {
            case TileType::QUESTION_COIN:
            case TileType::HIDDEN_COIN: {
                // Coin pop effect
                CoinPopEffect coinPop;
                coinPop.spawn(blockWorldX, blockWorldY);
                m_coinPopEffects.push_back(coinPop);
                m_mario.coins++;
                if (m_mario.coins >= Constants::COINS_FOR_1UP) {
                    m_mario.coins -= Constants::COINS_FOR_1UP;
                    m_mario.lives++;
                }
                addScore(200);
                break;
            }
            case TileType::QUESTION_MUSHROOM: {
                if (m_mario.getPowerState() == PowerState::SMALL) {
                    auto mushroom = std::make_unique<Mushroom>();
                    mushroom->startEmerge(blockWorldX, blockWorldY);
                    m_mushrooms.push_back(std::move(mushroom));
                } else {
                    auto flower = std::make_unique<FireFlower>();
                    flower->startEmerge(blockWorldX, blockWorldY);
                    m_fireFlowers.push_back(std::move(flower));
                }
                break;
            }
            case TileType::QUESTION_STAR: {
                auto star = std::make_unique<Starman>();
                star->startEmerge(blockWorldX, blockWorldY);
                m_stars.push_back(std::move(star));
                break;
            }
            case TileType::QUESTION_1UP:
            case TileType::HIDDEN_1UP: {
                auto mushroom = std::make_unique<Mushroom>(true);
                mushroom->startEmerge(blockWorldX, blockWorldY);
                m_mushrooms.push_back(std::move(mushroom));
                break;
            }
            default:
                break;
        }
    }
}

void PlayState::render(SDL_Renderer* renderer) {
    float camX = m_camera.getX();
    float camY = m_camera.getY();

    // Render world
    renderWorld(renderer);

    // Render entities
    renderEntities(renderer);

    // Render effects
    renderEffects(renderer);

    // Render HUD
    renderHUD(renderer);
}

void PlayState::renderWorld(SDL_Renderer* renderer) {
    float camX = m_camera.getX();
    float camY = m_camera.getY();

    // Background decorations first
    m_level.getTilemap().renderBackground(renderer, &m_tilesetSheet, camX, camY);

    // Main tilemap
    m_level.getTilemap().render(renderer, &m_tilesetSheet, camX, camY);
}

void PlayState::renderEntities(SDL_Renderer* renderer) {
    float camX = m_camera.getX();
    float camY = m_camera.getY();

    // Items behind enemies
    for (auto& mushroom : m_mushrooms) {
        if (mushroom->alive) mushroom->render(renderer, camX, camY);
    }
    for (auto& flower : m_fireFlowers) {
        if (flower->alive) flower->render(renderer, camX, camY);
    }
    for (auto& star : m_stars) {
        if (star->alive) star->render(renderer, camX, camY);
    }
    for (auto& coin : m_coins) {
        if (coin->alive) coin->render(renderer, camX, camY);
    }

    // Enemies
    for (auto& goomba : m_goombas) {
        if (goomba->alive && goomba->active) goomba->render(renderer, camX, camY);
    }
    for (auto& koopa : m_koopas) {
        if (koopa->alive && koopa->active) koopa->render(renderer, camX, camY);
    }
    for (auto& piranha : m_piranhas) {
        if (piranha->alive && piranha->active) piranha->render(renderer, camX, camY);
    }

    // Mario (rendered last, on top)
    m_mario.render(renderer, camX, camY);

    // Fireballs on top of everything
    for (auto& fireball : m_fireballs) {
        if (fireball->alive) fireball->render(renderer, camX, camY);
    }
}

void PlayState::renderEffects(SDL_Renderer* renderer) {
    float camX = m_camera.getX();
    float camY = m_camera.getY();

    // Brick break particles
    for (auto& effect : m_brickEffects) {
        if (!effect.active) continue;
        for (auto& p : effect.particles) {
            if (!p.active) continue;
            // Render as small brown squares (placeholder — will use sprite later)
            SDL_SetRenderDrawColor(renderer, 0xD0, 0x70, 0x30, 255);
            SDL_Rect rect = {
                static_cast<int>(p.x - camX),
                static_cast<int>(p.y - camY),
                8, 8
            };
            SDL_RenderFillRect(renderer, &rect);
        }
    }

    // Coin pop effects
    for (auto& effect : m_coinPopEffects) {
        if (!effect.active) continue;
        SDL_SetRenderDrawColor(renderer, 0xF8, 0xB8, 0x00, 255);
        SDL_Rect rect = {
            static_cast<int>(effect.x - camX + 2),
            static_cast<int>(effect.y - camY),
            12, 14
        };
        SDL_RenderFillRect(renderer, &rect);
    }

    // Score popups (rendered as simple text position markers for now)
    // Full font rendering will be implemented with the HUD system
    for (auto& popup : m_scorePopups) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_Rect rect = {
            static_cast<int>(popup.x - camX),
            static_cast<int>(popup.y - camY),
            16, 8
        };
        SDL_RenderFillRect(renderer, &rect);
    }
}

void PlayState::renderHUD(SDL_Renderer* renderer) {
    // Placeholder HUD rendering with colored rectangles.
    // Full bitmap font rendering will replace this.

    // Top bar background (semi-transparent black)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_Rect hudBg = { 0, 0, Constants::NES_WIDTH, 24 };
    SDL_RenderFillRect(renderer, &hudBg);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    // TODO: Render actual text with bitmap font
    // MARIO     WORLD   TIME
    // 000000    1-1     400
}

void PlayState::addScorePopup(float x, float y, int value) {
    m_scorePopups.push_back({ x, y, value, 30 });
}

void PlayState::addScore(int points) {
    m_mario.score += points;
}
