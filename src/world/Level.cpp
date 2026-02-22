#include "world/Level.h"
#include "core/Constants.h"

Level::Level() {}

void Level::addGround(int startX, int endX, int y, int height) {
    for (int row = y; row < y + height && row < m_tilemap.getHeight(); row++) {
        for (int col = startX; col <= endX; col++) {
            m_tilemap.setTile(col, row, TileType::GROUND);
        }
    }
}

void Level::addPipe(int x, int y, int height, bool /* enterable */) {
    // Pipe top
    m_tilemap.setTile(x, y, TileType::PIPE_TOP_LEFT);
    m_tilemap.setTile(x + 1, y, TileType::PIPE_TOP_RIGHT);
    // Pipe body
    for (int row = y + 1; row < y + height; row++) {
        m_tilemap.setTile(x, row, TileType::PIPE_BODY_LEFT);
        m_tilemap.setTile(x + 1, row, TileType::PIPE_BODY_RIGHT);
    }
}

void Level::addStaircase(int startX, int baseY, int height, bool ascending) {
    for (int step = 0; step < height; step++) {
        int x = ascending ? startX + step : startX + (height - 1 - step);
        for (int row = baseY - step; row <= baseY; row++) {
            m_tilemap.setTile(x, row, TileType::STAIR_BLOCK);
        }
    }
}

void Level::addQuestionBlock(int x, int y, TileType contents) {
    m_tilemap.setTile(x, y, contents);
}

void Level::addBrick(int x, int y) {
    m_tilemap.setTile(x, y, TileType::BRICK);
}

void Level::addBrickRow(int startX, int endX, int y) {
    for (int x = startX; x <= endX; x++) {
        addBrick(x, y);
    }
}

void Level::buildWorld1_1() {
    // World 1-1 dimensions: 224 tiles wide × 15 tiles high
    // The level is slightly wider than the canonical 211 to include the castle area
    const int WIDTH = 224;
    const int HEIGHT = 15;

    m_tilemap.init(WIDTH, HEIGHT);
    m_timeLimit = 400;
    m_marioStartX = 40.0f;
    m_marioStartY = 192.0f;  // Row 12 * 16 = 192 (standing on ground at row 13)

    // === GROUND LAYER ===
    // Ground rows are 13 and 14 (bottom two rows of the 15-row screen)
    // Gaps (pits) are placed by NOT adding ground

    // Ground segment 1: start to first pit
    addGround(0, 68, 13, 2);

    // Gap at tiles 69-70

    // Ground segment 2
    addGround(71, 85, 13, 2);

    // Gap at tiles 86-88

    // Ground segment 3
    addGround(89, 152, 13, 2);

    // Gap at tiles 153-154

    // Ground segment 4 (to the end)
    addGround(155, 223, 13, 2);

    // === QUESTION BLOCKS ===
    // Single ? block (coin)
    addQuestionBlock(16, 9, TileType::QUESTION_COIN);

    // Block cluster: BRICK ? BRICK ? BRICK at row 9
    addBrick(20, 9);
    addQuestionBlock(21, 9, TileType::QUESTION_MUSHROOM);  // Power-up!
    addBrick(22, 9);
    addQuestionBlock(23, 9, TileType::QUESTION_COIN);
    addBrick(24, 9);

    // High ? block (coin)
    addQuestionBlock(22, 5, TileType::QUESTION_COIN);

    // === PIPES ===
    // Pipe 1 (height 2)
    addPipe(28, 11, 2);

    // Pipe 2 (height 3)
    addPipe(38, 10, 3);

    // Pipe 3 (height 4) — leads to underground bonus room
    addPipe(46, 9, 4, true);

    // Pipe 4 (height 4)
    addPipe(57, 9, 4);

    // === MORE BLOCKS ===
    // Block formation around tile 78
    addBrick(77, 9);
    addQuestionBlock(78, 9, TileType::QUESTION_MUSHROOM);
    addBrick(79, 9);

    // Brick row above (high)
    addBrick(80, 5);
    addBrick(81, 5);
    addBrick(82, 5);
    addBrick(83, 5);
    addBrick(84, 5);
    addBrick(85, 5);
    addBrick(86, 5);
    addBrick(87, 5);

    // Blocks in second section
    addBrick(91, 5);
    addBrick(92, 5);
    addBrick(93, 5);
    addQuestionBlock(94, 5, TileType::QUESTION_COIN);

    // 10-coin brick (multi-hit, timed — dispenses up to 10 coins)
    m_tilemap.setTile(94, 9, TileType::BRICK_COINS);

    // Starman brick (looks like a regular brick, contains Starman)
    m_tilemap.setTile(101, 9, TileType::BRICK_STAR);

    // Hidden 1-UP block
    m_tilemap.setTile(64, 9, TileType::HIDDEN_1UP);

    // Brick row with coins (around tile 106)
    addBrick(106, 9);
    addBrick(107, 9);
    addQuestionBlock(109, 9, TileType::QUESTION_COIN);
    addQuestionBlock(109, 5, TileType::QUESTION_COIN);

    addBrick(112, 9);

    // Brick row around tile 118
    addBrick(118, 5);
    addBrick(119, 5);
    addBrick(120, 5);

    addBrick(121, 9);
    addBrick(122, 9);
    addBrick(123, 9);
    addBrick(124, 9);

    addBrick(128, 9);
    addQuestionBlock(129, 9, TileType::QUESTION_COIN);
    addQuestionBlock(130, 9, TileType::QUESTION_COIN);
    addBrick(131, 9);

    // === STAIRCASES ===
    // First staircase (ascending, before first gap section)
    // 4-step staircase at tile 134
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j <= i; j++) {
            m_tilemap.setTile(134 + i, 12 - j, TileType::STAIR_BLOCK);
        }
    }

    // Descending staircase after gap
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j <= 3 - i; j++) {
            m_tilemap.setTile(140 + i, 12 - j, TileType::STAIR_BLOCK);
        }
    }

    // Second staircase pair (near tile 148)
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j <= i; j++) {
            m_tilemap.setTile(148 + i, 12 - j, TileType::STAIR_BLOCK);
        }
    }
    // Gap at 153-154 (already no ground)
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j <= 3 - i; j++) {
            m_tilemap.setTile(155 + i, 12 - j, TileType::STAIR_BLOCK);
        }
    }

    // Brick row for coin running area
    addBrickRow(160, 168, 9);

    // === FINAL STAIRCASE (to flagpole) ===
    // 8-step ascending staircase
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j <= i; j++) {
            m_tilemap.setTile(181 + i, 12 - j, TileType::STAIR_BLOCK);
        }
    }

    // End block at top of staircase
    m_tilemap.setTile(189, 4, TileType::STAIR_BLOCK);

    // === FLAGPOLE ===
    m_tilemap.setTile(198, 3, TileType::FLAGPOLE_TOP);
    for (int row = 4; row <= 12; row++) {
        m_tilemap.setTile(198, row, TileType::FLAGPOLE_SHAFT);
    }

    // === CASTLE ===
    // Simple castle structure at the end
    int castleX = 202;
    // Castle body
    for (int row = 8; row <= 12; row++) {
        for (int col = castleX; col <= castleX + 4; col++) {
            m_tilemap.setTile(col, row, TileType::CASTLE_BLOCK);
        }
    }
    // Castle battlements
    m_tilemap.setTile(castleX, 7, TileType::CASTLE_BATTLEMENT);
    m_tilemap.setTile(castleX + 1, 7, TileType::CASTLE_BATTLEMENT);
    m_tilemap.setTile(castleX + 2, 6, TileType::CASTLE_BATTLEMENT);
    m_tilemap.setTile(castleX + 3, 7, TileType::CASTLE_BATTLEMENT);
    m_tilemap.setTile(castleX + 4, 7, TileType::CASTLE_BATTLEMENT);
    m_tilemap.setTile(castleX + 2, 7, TileType::CASTLE_BLOCK);
    // Castle door
    m_tilemap.setTile(castleX + 2, 11, TileType::CASTLE_DOOR);
    m_tilemap.setTile(castleX + 2, 12, TileType::CASTLE_DOOR);
    // Castle window
    m_tilemap.setTile(castleX + 1, 9, TileType::CASTLE_WINDOW);
    m_tilemap.setTile(castleX + 3, 9, TileType::CASTLE_WINDOW);

    // === BACKGROUND DECORATIONS ===
    // Hills (large hill near start)
    auto addHill = [this](int baseX, int baseY, int size) {
        // size 1 = small hill, size 2 = large hill
        m_tilemap.setTile(baseX, baseY, TileType::HILL_TOP);
        if (size >= 2) {
            m_tilemap.setTile(baseX - 1, baseY + 1, TileType::HILL_LEFT);
            m_tilemap.setTile(baseX, baseY + 1, TileType::HILL_SPOT);
            m_tilemap.setTile(baseX + 1, baseY + 1, TileType::HILL_RIGHT);
            if (size >= 2) {
                m_tilemap.setTile(baseX - 2, baseY + 2, TileType::HILL_LEFT);
                m_tilemap.setTile(baseX - 1, baseY + 2, TileType::HILL_FILL);
                m_tilemap.setTile(baseX, baseY + 2, TileType::HILL_FILL);
                m_tilemap.setTile(baseX + 1, baseY + 2, TileType::HILL_FILL);
                m_tilemap.setTile(baseX + 2, baseY + 2, TileType::HILL_RIGHT);
            }
        } else {
            m_tilemap.setTile(baseX - 1, baseY + 1, TileType::HILL_LEFT);
            m_tilemap.setTile(baseX + 1, baseY + 1, TileType::HILL_RIGHT);
            m_tilemap.setTile(baseX, baseY + 1, TileType::HILL_FILL);
        }
    };

    // Large hills
    addHill(0, 10, 2);
    addHill(48, 10, 2);
    addHill(96, 10, 2);
    addHill(144, 10, 2);
    addHill(192, 10, 2);

    // Small hills
    addHill(16, 11, 1);
    addHill(64, 11, 1);
    addHill(112, 11, 1);
    addHill(160, 11, 1);
    addHill(208, 11, 1);

    // Bushes (same shape as clouds, different palette)
    auto addBush = [this](int x, int y, int size) {
        if (size >= 3) {
            m_tilemap.setTile(x - 1, y, TileType::BUSH_LEFT);
            m_tilemap.setTile(x, y, TileType::BUSH_MID);
            m_tilemap.setTile(x + 1, y, TileType::BUSH_RIGHT);
        } else if (size >= 2) {
            m_tilemap.setTile(x, y, TileType::BUSH_LEFT);
            m_tilemap.setTile(x + 1, y, TileType::BUSH_RIGHT);
        } else {
            m_tilemap.setTile(x, y, TileType::BUSH_MID);
        }
    };

    addBush(11, 12, 3);
    addBush(35, 12, 1);
    addBush(59, 12, 3);
    addBush(83, 12, 1);
    addBush(107, 12, 3);
    addBush(131, 12, 1);
    addBush(155, 12, 3);

    // Clouds
    auto addCloud = [this](int x, int y, int size) {
        m_tilemap.setTile(x, y, TileType::CLOUD_MID);
        m_tilemap.setTile(x, y + 1, TileType::CLOUD_BOTTOM_MID);
        if (size >= 2) {
            m_tilemap.setTile(x - 1, y, TileType::CLOUD_LEFT);
            m_tilemap.setTile(x + 1, y, TileType::CLOUD_RIGHT);
            m_tilemap.setTile(x - 1, y + 1, TileType::CLOUD_BOTTOM_LEFT);
            m_tilemap.setTile(x + 1, y + 1, TileType::CLOUD_BOTTOM_RIGHT);
        }
        if (size >= 3) {
            m_tilemap.setTile(x + 2, y, TileType::CLOUD_RIGHT);
            m_tilemap.setTile(x + 2, y + 1, TileType::CLOUD_BOTTOM_RIGHT);
            // Shift right cloud
            m_tilemap.setTile(x + 1, y, TileType::CLOUD_MID);
            m_tilemap.setTile(x + 1, y + 1, TileType::CLOUD_BOTTOM_MID);
        }
    };

    addCloud(8, 3, 1);
    addCloud(19, 2, 1);
    addCloud(27, 3, 3);
    addCloud(36, 2, 2);
    addCloud(56, 3, 1);
    addCloud(67, 2, 1);
    addCloud(75, 3, 3);
    addCloud(84, 2, 2);
    addCloud(104, 3, 1);
    addCloud(115, 2, 1);
    addCloud(123, 3, 3);
    addCloud(132, 2, 2);

    // === ENEMY SPAWNS ===
    // Goombas
    m_enemySpawns.push_back({ EnemySpawn::GOOMBA, 22 * 16.0f, 12 * 16.0f });
    m_enemySpawns.push_back({ EnemySpawn::GOOMBA, 40 * 16.0f, 12 * 16.0f });
    m_enemySpawns.push_back({ EnemySpawn::GOOMBA, 51 * 16.0f, 12 * 16.0f });
    m_enemySpawns.push_back({ EnemySpawn::GOOMBA, 52.5f * 16.0f, 12 * 16.0f });
    m_enemySpawns.push_back({ EnemySpawn::GOOMBA, 80 * 16.0f, 4 * 16.0f });
    m_enemySpawns.push_back({ EnemySpawn::GOOMBA, 82 * 16.0f, 12 * 16.0f });
    m_enemySpawns.push_back({ EnemySpawn::GOOMBA, 97 * 16.0f, 12 * 16.0f });
    m_enemySpawns.push_back({ EnemySpawn::GOOMBA, 99 * 16.0f, 12 * 16.0f });
    m_enemySpawns.push_back({ EnemySpawn::GOOMBA, 114 * 16.0f, 12 * 16.0f });
    m_enemySpawns.push_back({ EnemySpawn::GOOMBA, 116 * 16.0f, 12 * 16.0f });
    m_enemySpawns.push_back({ EnemySpawn::GOOMBA, 124 * 16.0f, 12 * 16.0f });
    m_enemySpawns.push_back({ EnemySpawn::GOOMBA, 126 * 16.0f, 12 * 16.0f });
    m_enemySpawns.push_back({ EnemySpawn::GOOMBA, 170 * 16.0f, 12 * 16.0f });
    m_enemySpawns.push_back({ EnemySpawn::GOOMBA, 172 * 16.0f, 12 * 16.0f });

    // Koopa Troopa
    m_enemySpawns.push_back({ EnemySpawn::KOOPA, 107 * 16.0f, 11 * 16.0f });

    // NOTE: World 1-1 is the ONLY level with NO Piranha Plants.
    // (Confirmed by SMB disassembly research)
}
