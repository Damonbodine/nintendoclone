#pragma once

#include "states/GameState.h"
#include "entities/Mario.h"
#include "entities/Goomba.h"
#include "entities/KoopaTroopa.h"
#include "entities/PiranhaPlant.h"
#include "entities/Mushroom.h"
#include "entities/FireFlower.h"
#include "entities/Starman.h"
#include "entities/Fireball.h"
#include "entities/Coin.h"
#include "world/Level.h"
#include "world/Camera.h"
#include "world/Block.h"
#include "graphics/SpriteSheet.h"
#include <vector>
#include <memory>

class Game;

class PlayState : public GameState {
public:
    PlayState(Game& game);

    void enter() override;
    void exit() override;
    void update(Game& game) override;
    void render(SDL_Renderer* renderer) override;

private:
    Game& m_game;
    Level m_level;
    Camera m_camera;
    Mario m_mario;

    // Entity lists
    std::vector<std::unique_ptr<Goomba>> m_goombas;
    std::vector<std::unique_ptr<KoopaTroopa>> m_koopas;
    std::vector<std::unique_ptr<PiranhaPlant>> m_piranhas;
    std::vector<std::unique_ptr<Mushroom>> m_mushrooms;
    std::vector<std::unique_ptr<FireFlower>> m_fireFlowers;
    std::vector<std::unique_ptr<Starman>> m_stars;
    std::vector<std::unique_ptr<Fireball>> m_fireballs;
    std::vector<std::unique_ptr<Coin>> m_coins;

    // Visual effects
    std::vector<BrickBreakEffect> m_brickEffects;
    std::vector<CoinPopEffect> m_coinPopEffects;

    // Sprite sheets
    SpriteSheet m_tilesetSheet;
    SpriteSheet m_marioSmallSheet;
    SpriteSheet m_marioBigSheet;
    SpriteSheet m_enemySheet;
    SpriteSheet m_itemSheet;

    // HUD
    int m_timer = 400;
    int m_timerCounter = 0;  // Counts frames for timer decrement
    bool m_paused = false;

    // Score popup
    struct ScorePopup {
        float x, y;
        int value;
        int timer;
    };
    std::vector<ScorePopup> m_scorePopups;

    // Level complete
    bool m_levelComplete = false;
    int m_flagpoleTimer = 0;

    // HUD font
    SpriteSheet m_fontSheet;
    std::string m_fontCharMap;

    // Methods
    void loadAssets();
    void loadFont();
    void setupMarioAnimations();
    void setupEnemyAnimations(Goomba& goomba);
    void setupKoopaAnimations(KoopaTroopa& koopa);
    void setupPiranhaAnimations(PiranhaPlant& piranha);
    void setupMushroomSprite(Mushroom& mushroom);
    void setupItemSprite(Entity& entity, int frameIndex);
    void spawnEnemies();
    void spawnItems();
    void drawText(SDL_Renderer* renderer, const std::string& text, int x, int y);

    void updateMario(Game& game);
    void updateEnemies();
    void updateItems();
    void updateEffects();
    void updateHUD();

    void checkMarioEnemyCollisions();
    void checkMarioItemCollisions();
    void checkFireballCollisions();
    void checkShellCollisions();
    void activateEntities();
    void handleBlockHit(int tileX, int tileY);

    void renderWorld(SDL_Renderer* renderer);
    void renderEntities(SDL_Renderer* renderer);
    void renderEffects(SDL_Renderer* renderer);
    void renderHUD(SDL_Renderer* renderer);

    void addScorePopup(float x, float y, int value);
    void addScore(int points);
};
