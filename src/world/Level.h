#pragma once

#include "world/Tilemap.h"
#include "entities/Goomba.h"
#include "entities/KoopaTroopa.h"
#include "entities/PiranhaPlant.h"
#include "entities/Coin.h"
#include <vector>
#include <memory>

// Level holds the tilemap and all entity spawn data.
// Provides a factory method for building World 1-1 programmatically.
class Level {
public:
    Level();

    // Build World 1-1 (hardcoded, accurate layout)
    void buildWorld1_1();

    Tilemap& getTilemap() { return m_tilemap; }
    const Tilemap& getTilemap() const { return m_tilemap; }

    // Entity spawn definitions (position + type)
    struct EnemySpawn {
        enum Type { GOOMBA, KOOPA, PIRANHA };
        Type type;
        float x, y;
    };

    struct CoinSpawn {
        float x, y;
    };

    const std::vector<EnemySpawn>& getEnemySpawns() const { return m_enemySpawns; }
    const std::vector<CoinSpawn>& getCoinSpawns() const { return m_coinSpawns; }

    int getTimeLimit() const { return m_timeLimit; }
    float getMarioStartX() const { return m_marioStartX; }
    float getMarioStartY() const { return m_marioStartY; }

private:
    Tilemap m_tilemap;
    std::vector<EnemySpawn> m_enemySpawns;
    std::vector<CoinSpawn> m_coinSpawns;
    int m_timeLimit = 400;
    float m_marioStartX = 40.0f;
    float m_marioStartY = 192.0f;

    // Helper methods for building the level
    void addGround(int startX, int endX, int y, int height = 2);
    void addPipe(int x, int y, int height, bool enterable = false);
    void addStaircase(int startX, int baseY, int height, bool ascending = true);
    void addQuestionBlock(int x, int y, TileType contents);
    void addBrick(int x, int y);
    void addBrickRow(int startX, int endX, int y);
};
