#include "states/PlayState.h"
#include "states/TitleState.h"
#include "core/Game.h"
#include "core/Constants.h"
#include "physics/Collision.h"
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <sstream>

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
    loadFont();
    setupMarioAnimations();

    // Spawn entities from level data
    spawnEnemies();

    // Wire up sprite animations for all spawned enemies
    for (auto& g : m_goombas) setupEnemyAnimations(*g);
    for (auto& k : m_koopas) setupKoopaAnimations(*k);
    for (auto& p : m_piranhas) setupPiranhaAnimations(*p);

    // Init timer
    m_timer = m_level.getTimeLimit();
    m_timerCounter = 0;

    // Start background music
    m_game.getAudio().playMusic("overworld");
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
    // Small Mario: 16x16 per frame, laid out horizontally
    // Frame 0: idle, 1-3: walk, 4: jump, 5: skid, 6: die
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

    // Big Mario: 16x32 per frame, same frame layout
    if (m_marioBigSheet.getTexture()) {
        Animation bigIdle;
        bigIdle.init(&m_marioBigSheet, { {0, 1} });
        Animation bigWalk;
        bigWalk.init(&m_marioBigSheet, { {1, 6}, {2, 6}, {3, 6} });
        Animation bigJump;
        bigJump.init(&m_marioBigSheet, { {4, 1} });
        Animation bigSkid;
        bigSkid.init(&m_marioBigSheet, { {5, 1} });
        Animation bigDie;
        bigDie.init(&m_marioBigSheet, { {6, 1} }, false);
        // Crouch reuses the skid frame for now
        Animation bigCrouch;
        bigCrouch.init(&m_marioBigSheet, { {5, 1} });

        m_mario.sprite.addAnimation("big_idle", bigIdle);
        m_mario.sprite.addAnimation("big_walk", bigWalk);
        m_mario.sprite.addAnimation("big_jump", bigJump);
        m_mario.sprite.addAnimation("big_skid", bigSkid);
        m_mario.sprite.addAnimation("big_die", bigDie);
        m_mario.sprite.addAnimation("big_crouch", bigCrouch);
    }

    m_lastPowerState = m_mario.getPowerState();
}

void PlayState::loadFont() {
    auto& resources = m_game.getResources();
    SDL_Texture* fontTex = resources.loadTexture("assets/fonts/nes_font.png");
    if (fontTex) {
        m_fontSheet.init(fontTex, 8, 8);
    }
    // Read character map
    std::ifstream mapFile("assets/fonts/nes_font_map.txt");
    if (mapFile.is_open()) {
        std::getline(mapFile, m_fontCharMap);
    }
}

void PlayState::setupEnemyAnimations(Goomba& goomba) {
    if (!m_enemySheet.getTexture()) return;
    // Enemy sheet layout: 0=goomba_walk1, 1=goomba_walk2, 2=goomba_squish
    Animation walk;
    walk.init(&m_enemySheet, { {0, 8}, {1, 8} });
    Animation squished;
    squished.init(&m_enemySheet, { {2, 1} }, false);

    goomba.sprite.addAnimation("walk", walk);
    goomba.sprite.addAnimation("squished", squished);
    goomba.sprite.setAnimation("walk");
}

void PlayState::setupKoopaAnimations(KoopaTroopa& koopa) {
    if (!m_enemySheet.getTexture()) return;
    // Enemy sheet: 3=koopa_walk1, 4=koopa_walk2, 5=shell, 6-9=spin
    Animation walk;
    walk.init(&m_enemySheet, { {3, 8}, {4, 8} });
    Animation shellIdle;
    shellIdle.init(&m_enemySheet, { {5, 1} });
    Animation shellSpin;
    shellSpin.init(&m_enemySheet, { {6, 2}, {7, 2}, {8, 2}, {9, 2} });

    koopa.sprite.addAnimation("walk", walk);
    koopa.sprite.addAnimation("shell_idle", shellIdle);
    koopa.sprite.addAnimation("shell_spin", shellSpin);
    koopa.sprite.setAnimation("walk");
}

void PlayState::setupPiranhaAnimations(PiranhaPlant& piranha) {
    if (!m_enemySheet.getTexture()) return;
    // Enemy sheet: 10=piranha_closed, 11=piranha_open
    Animation chomp;
    chomp.init(&m_enemySheet, { {10, 8}, {11, 8} });

    piranha.sprite.addAnimation("chomp", chomp);
    piranha.sprite.setAnimation("chomp");
}

void PlayState::setupMushroomSprite(Mushroom& mushroom) {
    if (!m_itemSheet.getTexture()) return;
    // Items sheet: 0=mushroom, 4=1up_mushroom
    int frame = mushroom.isOneUp() ? 4 : 0;
    Animation idle;
    idle.init(&m_itemSheet, { {frame, 1} });
    mushroom.sprite.addAnimation("idle", idle);
    mushroom.sprite.setAnimation("idle");
}

void PlayState::setupItemSprite(Entity& entity, int frameIndex) {
    if (!m_itemSheet.getTexture()) return;
    Animation idle;
    idle.init(&m_itemSheet, { {frameIndex, 1} });
    entity.sprite.addAnimation("idle", idle);
    entity.sprite.setAnimation("idle");
}

void PlayState::drawText(SDL_Renderer* renderer, const std::string& text, int x, int y) {
    if (!m_fontSheet.getTexture() || m_fontCharMap.empty()) return;

    for (size_t i = 0; i < text.size(); i++) {
        char ch = text[i];
        // Convert lowercase to uppercase
        if (ch >= 'a' && ch <= 'z') ch = ch - 'a' + 'A';

        size_t idx = m_fontCharMap.find(ch);
        if (idx == std::string::npos) {
            // Unknown char — skip (renders as blank space)
            x += 8;
            continue;
        }

        SDL_Rect src = m_fontSheet.getFrameRect(static_cast<int>(idx));
        SDL_Rect dst = { x + static_cast<int>(i) * 8, y, 8, 8 };
        SDL_RenderCopy(renderer, m_fontSheet.getTexture(), &src, &dst);
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
    if (game.getInput().isPressed(Input::START) && !m_mario.isDead() && !m_gameOver) {
        m_paused = true;
        return;
    }

    // Game over screen — wait then return to title
    if (m_gameOver) {
        m_gameOverTimer++;
        if (m_gameOverTimer > 180 ||
            (m_gameOverTimer > 60 && game.getInput().isPressed(Input::START))) {
            game.changeState(std::make_unique<TitleState>(game));
        }
        return;
    }

    // Level complete sequence
    if (m_levelComplete) {
        m_flagpoleTimer++;
        // Mario walks right toward the castle
        if (m_flagpoleTimer > 30 && m_flagpoleTimer < 180) {
            m_mario.x += 1.0f;
            m_mario.vx = 1.0f;
        }
        // Transition back to title after sequence
        if (m_flagpoleTimer > 240 ||
            (m_flagpoleTimer > 60 && game.getInput().isPressed(Input::START))) {
            game.changeState(std::make_unique<TitleState>(game));
        }
        return;
    }

    // Death sequence — wait for animation, then respawn or game over
    if (m_mario.isDead()) {
        m_deathTimer++;
        // Let death animation play (bounce up and fall off-screen)
        m_mario.update(m_level.getTilemap());
        if (m_deathTimer > 180) {
            if (m_mario.lives <= 0) {
                m_gameOver = true;
                m_gameOverTimer = 0;
            } else {
                // Respawn: reload the level
                game.changeState(std::make_unique<PlayState>(game));
            }
        }
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

    // Collision checks — run every frame for reliable stomp detection
    checkMarioEnemyCollisions();
    checkMarioItemCollisions();
    checkFireballCollisions();
    checkShellCollisions();

    // Update camera
    m_camera.update(m_mario.x, m_mario.y);

    // Update effects
    updateEffects();

    // Update HUD
    updateHUD();

    // Clean up dead fireballs and update fireball counter
    m_fireballs.erase(
        std::remove_if(m_fireballs.begin(), m_fireballs.end(),
            [](const std::unique_ptr<Fireball>& fb) { return !fb->alive; }),
        m_fireballs.end());
    m_mario.fireballCount = static_cast<int>(m_fireballs.size());
}

void PlayState::updateMario(Game& game) {
    m_mario.handleInput(game.getInput());
    m_mario.update(m_level.getTilemap());

    // Audio events from Mario
    auto& audio = m_game.getAudio();
    if (m_mario.justJumped) audio.playSound("jump");
    if (m_mario.justDied) {
        audio.stopMusic();
        audio.playSound("death");
    }

    // Fireball throwing (Fire Mario + B pressed)
    if (m_mario.canThrowFireball() && game.getInput().isPressed(Input::B)) {
        m_mario.throwFireball();
        auto fireball = std::make_unique<Fireball>();
        float fbX = m_mario.facingRight ? m_mario.x + 12 : m_mario.x - 8;
        fireball->spawn(fbX, m_mario.y + 8, m_mario.facingRight);
        fireball->active = true;
        // Set up fireball sprite
        if (m_itemSheet.getTexture()) {
            Animation active;
            active.init(&m_itemSheet, { {3, 4}, {3, 4} });
            fireball->sprite.addAnimation("active", active);
            fireball->sprite.addAnimation("explode", active);  // reuse for explode
            fireball->sprite.setAnimation("active");
        }
        m_fireballs.push_back(std::move(fireball));
    }

    // Swap sprite sheet when power state changes (small <-> big/fire)
    PowerState currentPower = m_mario.getPowerState();
    if (currentPower != m_lastPowerState) {
        bool wasBig = (m_lastPowerState != PowerState::SMALL);
        bool isBig = (currentPower != PowerState::SMALL);

        if (wasBig != isBig) {
            // Need to swap animation set (small <-> big prefix)
            std::string currentAnim = m_mario.sprite.getCurrentAnimationName();

            if (isBig) {
                // Switch to big_ prefix animations
                // Map: "idle" -> "big_idle", etc.
                if (currentAnim.find("big_") != 0) {
                    m_mario.sprite.setAnimation("big_" + currentAnim);
                }
            } else {
                // Switch back to small animations
                // Map: "big_idle" -> "idle", etc.
                if (currentAnim.find("big_") == 0) {
                    m_mario.sprite.setAnimation(currentAnim.substr(4));
                }
            }
        }
        m_lastPowerState = currentPower;
    }

    // Check for block hits from below.
    // Use preCollisionVy: after resolveCollisionWithTilemap(), vy is zeroed on ceiling hit.
    // Check the tile at Mario's head AND one tile above (collision pushed him down).
    if (m_mario.preCollisionVy < 0) {
        AABB marioBox = m_mario.getWorldBounds();
        int leftTile = static_cast<int>(marioBox.left()) / Constants::TILE_SIZE;
        int rightTile = static_cast<int>(marioBox.right() - 0.001f) / Constants::TILE_SIZE;
        int headTile = static_cast<int>(marioBox.top()) / Constants::TILE_SIZE;
        int aboveTile = headTile - 1;

        for (int checkY : {headTile, aboveTile}) {
            if (checkY < 0) continue;
            for (int tx = leftTile; tx <= rightTile; tx++) {
                TileType tile = m_level.getTilemap().getTile(tx, checkY);
                if (TileProperties::isInteractive(tile)) {
                    handleBlockHit(tx, checkY);
                }
            }
        }
    }

    // Check flagpole collision (level complete)
    if (!m_levelComplete) {
        AABB marioBox = m_mario.getWorldBounds();
        int marioTileX = static_cast<int>(marioBox.centerX()) / Constants::TILE_SIZE;
        int marioTileY = static_cast<int>(marioBox.centerY()) / Constants::TILE_SIZE;

        // Check if Mario touches the flagpole column (check a 3-tile wide area)
        for (int tx = marioTileX - 1; tx <= marioTileX + 1; tx++) {
            for (int ty = marioTileY - 1; ty <= marioTileY + 1; ty++) {
                TileType tile = m_level.getTilemap().getTile(tx, ty);
                if (TileProperties::isFlagpole(tile)) {
                    m_levelComplete = true;
                    m_flagpoleTimer = 0;
                    m_mario.vx = 0;
                    addScore(2000);
                    break;
                }
            }
            if (m_levelComplete) break;
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

    // Use pre-collision velocity for stomp detection.
    // After resolveCollisionWithTilemap(), vy is zeroed when Mario lands on the
    // same ground tile as an enemy, which would make isStomping() always fail.
    float stompVy = m_mario.preCollisionVy;

    // Goombas
    for (auto& goomba : m_goombas) {
        if (!goomba->alive || !goomba->active || goomba->isSquished()) continue;

        AABB goombaBox = goomba->getWorldBounds();
        if (!marioBox.overlaps(goombaBox)) continue;

        if (Collision::isStomping(marioBox, goombaBox, stompVy)) {
            goomba->stomp();
            m_mario.vy = -4.0f;
            m_mario.stompCombo++;
            m_game.getAudio().playSound("stomp");

            int points = 100;
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
        if (koopa->isFlipped()) continue;

        AABB koopaBox = koopa->getWorldBounds();
        if (!marioBox.overlaps(koopaBox)) continue;

        if (Collision::isStomping(marioBox, koopaBox, stompVy)) {
            koopa->stomp();
            m_mario.vy = -4.0f;
            m_game.getAudio().playSound("stomp");

            if (koopa->getKoopaState() == KoopaState::WALKING) {
                addScore(100);
            }
        } else if (koopa->getKoopaState() == KoopaState::SHELL_IDLE) {
            bool kickRight = m_mario.x < koopa->x;
            koopa->kick(kickRight);
            addScore(400);
            m_game.getAudio().playSound("bump");
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
            } else {
                m_mario.grow();
                addScore(1000);
            }
            mushroom->alive = false;
            m_game.getAudio().playSound("powerup");
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
            m_game.getAudio().playSound("powerup");
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
            m_game.getAudio().playSound("powerup");
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
            m_game.getAudio().playSound("coin");
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
        BrickBreakEffect effect;
        effect.spawn(tileX * Constants::TILE_SIZE, tileY * Constants::TILE_SIZE);
        m_brickEffects.push_back(effect);
        m_game.getAudio().playSound("bump");
    }

    if (result.spawnsItem) {
        float blockWorldX = tileX * Constants::TILE_SIZE;
        float blockWorldY = tileY * Constants::TILE_SIZE;
        m_game.getAudio().playSound("coin");

        switch (result.originalType) {
            case TileType::QUESTION_COIN:
            case TileType::HIDDEN_COIN: {
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
                    setupMushroomSprite(*mushroom);
                    m_mushrooms.push_back(std::move(mushroom));
                } else {
                    auto flower = std::make_unique<FireFlower>();
                    flower->startEmerge(blockWorldX, blockWorldY);
                    setupItemSprite(*flower, 1);  // fire flower = frame 1
                    m_fireFlowers.push_back(std::move(flower));
                }
                break;
            }
            case TileType::QUESTION_STAR: {
                auto star = std::make_unique<Starman>();
                star->startEmerge(blockWorldX, blockWorldY);
                setupItemSprite(*star, 2);  // star = frame 2
                m_stars.push_back(std::move(star));
                break;
            }
            case TileType::QUESTION_1UP:
            case TileType::HIDDEN_1UP: {
                auto mushroom = std::make_unique<Mushroom>(true);
                mushroom->startEmerge(blockWorldX, blockWorldY);
                setupMushroomSprite(*mushroom);
                m_mushrooms.push_back(std::move(mushroom));
                break;
            }
            default:
                break;
        }
    }
}

void PlayState::render(SDL_Renderer* renderer) {
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

    // Score popups — render as text
    for (auto& popup : m_scorePopups) {
        if (popup.value > 0) {
            char buf[8];
            std::snprintf(buf, sizeof(buf), "%d", popup.value);
            drawText(renderer, buf,
                     static_cast<int>(popup.x - camX),
                     static_cast<int>(popup.y - camY));
        }
    }
}

void PlayState::renderHUD(SDL_Renderer* renderer) {
    // NES SMB HUD layout (at NES resolution 256x240):
    // Row 1 (y=8):  MARIO         WORLD    TIME
    // Row 2 (y=16): 000000  x00   1-1       400

    // Labels
    drawText(renderer, "MARIO", 24, 8);
    drawText(renderer, "WORLD", 144, 8);
    drawText(renderer, "TIME", 200, 8);

    // Score (6 digits, zero-padded)
    char scoreBuf[16];
    std::snprintf(scoreBuf, sizeof(scoreBuf), "%06d", m_mario.score);
    drawText(renderer, scoreBuf, 24, 16);

    // Coin count
    char coinBuf[8];
    std::snprintf(coinBuf, sizeof(coinBuf), "x%02d", m_mario.coins);
    drawText(renderer, coinBuf, 96, 16);

    // World number
    drawText(renderer, "1-1", 152, 16);

    // Timer (3 digits)
    char timerBuf[8];
    std::snprintf(timerBuf, sizeof(timerBuf), "%3d", m_timer);
    drawText(renderer, timerBuf, 208, 16);

    // Lives count (bottom-left area or next to coins)
    char livesBuf[8];
    std::snprintf(livesBuf, sizeof(livesBuf), "x%d", m_mario.lives);
    drawText(renderer, livesBuf, 72, 16);

    // Game over overlay
    if (m_gameOver) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_Rect overlay = { 0, 0, Constants::NES_WIDTH, Constants::NES_HEIGHT };
        SDL_RenderFillRect(renderer, &overlay);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        drawText(renderer, "GAME OVER", 88, 112);
    }

    // Level complete overlay
    if (m_levelComplete && m_flagpoleTimer > 60) {
        drawText(renderer, "COURSE CLEAR", 72, 80);
        char scoreBuf2[24];
        std::snprintf(scoreBuf2, sizeof(scoreBuf2), "YOU GOT %d", m_mario.score);
        drawText(renderer, scoreBuf2, 64, 100);
    }
}

void PlayState::addScorePopup(float x, float y, int value) {
    m_scorePopups.push_back({ x, y, value, 30 });
}

void PlayState::addScore(int points) {
    m_mario.score += points;
}
