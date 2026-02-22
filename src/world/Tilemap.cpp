#include "world/Tilemap.h"
#include "graphics/SpriteSheet.h"
#include "core/Constants.h"
#include <fstream>
#include <cmath>
#include <algorithm>
#include <cstdio>

Tilemap::Tilemap() {}

void Tilemap::init(int width, int height) {
    m_width = width;
    m_height = height;
    m_tiles.resize(width * height, TileType::EMPTY);
    m_background.resize(width * height, TileType::EMPTY);
}

bool Tilemap::loadFromFile(const std::string& path) {
    // TODO: Implement JSON level loading
    // For now, we build levels programmatically
    std::fprintf(stderr, "Level loading from file not yet implemented: %s\n", path.c_str());
    return false;
}

void Tilemap::setTile(int x, int y, TileType type) {
    if (inBounds(x, y)) {
        if (TileProperties::isBackground(type)) {
            m_background[tileIndex(x, y)] = type;
        } else {
            m_tiles[tileIndex(x, y)] = type;
        }
    }
}

TileType Tilemap::getTile(int x, int y) const {
    if (!inBounds(x, y)) return TileType::EMPTY;
    return m_tiles[tileIndex(x, y)];
}

bool Tilemap::isSolid(int tileX, int tileY) const {
    if (!inBounds(tileX, tileY)) {
        // Below the level = not solid (fall to death)
        // Above the level = not solid
        // Left of level = solid (can't go back)
        if (tileX < 0) return true;
        return false;
    }
    return TileProperties::isSolid(m_tiles[tileIndex(tileX, tileY)]);
}

void Tilemap::render(SDL_Renderer* renderer, SpriteSheet* tileset,
                     float cameraX, float cameraY) {
    if (!tileset) return;

    // Calculate visible tile range
    int startX = std::max(0, static_cast<int>(cameraX / Constants::TILE_SIZE));
    int endX = std::min(m_width, static_cast<int>((cameraX + Constants::NES_WIDTH) / Constants::TILE_SIZE) + 1);
    int startY = std::max(0, static_cast<int>(cameraY / Constants::TILE_SIZE));
    int endY = std::min(m_height, static_cast<int>((cameraY + Constants::NES_HEIGHT) / Constants::TILE_SIZE) + 1);

    for (int y = startY; y < endY; y++) {
        for (int x = startX; x < endX; x++) {
            TileType type = m_tiles[tileIndex(x, y)];
            if (type == TileType::EMPTY) continue;
            if (TileProperties::isBackground(type)) continue;

            int frame = getTileFrame(type);
            if (frame < 0) continue;

            // Check for bump animation offset
            float bumpOffset = 0;
            for (const auto& bump : m_bumpAnims) {
                if (bump.tileX == x && bump.tileY == y) {
                    bumpOffset = bump.offsetY;
                    break;
                }
            }

            SDL_Rect src = tileset->getFrameRect(frame);
            SDL_Rect dst = {
                static_cast<int>(x * Constants::TILE_SIZE - cameraX),
                static_cast<int>(y * Constants::TILE_SIZE - cameraY + bumpOffset),
                Constants::TILE_SIZE,
                Constants::TILE_SIZE
            };
            SDL_RenderCopy(renderer, tileset->getTexture(), &src, &dst);
        }
    }
}

void Tilemap::renderBackground(SDL_Renderer* renderer, SpriteSheet* tileset,
                                float cameraX, float cameraY) {
    if (!tileset) return;

    int startX = std::max(0, static_cast<int>(cameraX / Constants::TILE_SIZE));
    int endX = std::min(m_width, static_cast<int>((cameraX + Constants::NES_WIDTH) / Constants::TILE_SIZE) + 1);
    int startY = std::max(0, static_cast<int>(cameraY / Constants::TILE_SIZE));
    int endY = std::min(m_height, static_cast<int>((cameraY + Constants::NES_HEIGHT) / Constants::TILE_SIZE) + 1);

    for (int y = startY; y < endY; y++) {
        for (int x = startX; x < endX; x++) {
            TileType type = m_background[tileIndex(x, y)];
            if (type == TileType::EMPTY) continue;

            int frame = getTileFrame(type);
            if (frame < 0) continue;

            SDL_Rect src = tileset->getFrameRect(frame);
            SDL_Rect dst = {
                static_cast<int>(x * Constants::TILE_SIZE - cameraX),
                static_cast<int>(y * Constants::TILE_SIZE - cameraY),
                Constants::TILE_SIZE,
                Constants::TILE_SIZE
            };
            SDL_RenderCopy(renderer, tileset->getTexture(), &src, &dst);
        }
    }
}

Tilemap::BlockHitResult Tilemap::hitBlockFromBelow(int tileX, int tileY, bool isBigMario) {
    BlockHitResult result;
    if (!inBounds(tileX, tileY)) return result;

    TileType type = m_tiles[tileIndex(tileX, tileY)];
    result.originalType = type;
    result.tileX = tileX;
    result.tileY = tileY;

    if (TileProperties::isQuestion(type) || TileProperties::isHidden(type)) {
        // Turn into used block
        m_tiles[tileIndex(tileX, tileY)] = TileType::USED_BLOCK;
        result.spawnsItem = true;

        // Start bump animation
        m_bumpAnims.push_back({ tileX, tileY, 16, 0.0f });
    } else if (type == TileType::BRICK_COINS) {
        // Multi-coin brick: dispenses up to 10 coins, has countdown timer
        int idx = tileIndex(tileX, tileY);
        if (m_coinBrickCounters.find(idx) == m_coinBrickCounters.end()) {
            m_coinBrickCounters[idx] = 10;
            m_coinBrickTimers[idx] = 300;  // ~5 seconds to hit all coins
        }

        m_coinBrickCounters[idx]--;
        m_coinBrickTimers[idx] -= 30;  // Each hit reduces timer

        result.spawnsItem = true;
        result.originalType = TileType::QUESTION_COIN;  // Spawn a coin

        if (m_coinBrickCounters[idx] <= 0 || m_coinBrickTimers[idx] <= 0) {
            // Out of coins or timer expired — become used block
            m_tiles[idx] = TileType::USED_BLOCK;
            m_coinBrickCounters.erase(idx);
            m_coinBrickTimers.erase(idx);
        }

        m_bumpAnims.push_back({ tileX, tileY, 16, 0.0f });
    } else if (type == TileType::BRICK_STAR) {
        // Star brick: looks like a brick but contains a Starman
        m_tiles[tileIndex(tileX, tileY)] = TileType::USED_BLOCK;
        result.spawnsItem = true;
        result.originalType = TileType::QUESTION_STAR;  // Spawn a star

        m_bumpAnims.push_back({ tileX, tileY, 16, 0.0f });
    } else if (TileProperties::isBrick(type)) {
        // Regular brick
        if (isBigMario) {
            // Big Mario breaks bricks
            m_tiles[tileIndex(tileX, tileY)] = TileType::EMPTY;
            result.breaksBrick = true;
        } else {
            // Small Mario just bumps bricks
            m_bumpAnims.push_back({ tileX, tileY, 16, 0.0f });
        }
    }

    return result;
}

void Tilemap::updateAnimations() {
    m_globalAnimTimer++;

    // Update multi-coin brick timers (expire → become used block)
    for (auto it = m_coinBrickTimers.begin(); it != m_coinBrickTimers.end(); ) {
        it->second--;
        if (it->second <= 0) {
            int idx = it->first;
            m_tiles[idx] = TileType::USED_BLOCK;
            m_coinBrickCounters.erase(idx);
            it = m_coinBrickTimers.erase(it);
        } else {
            ++it;
        }
    }

    // Update bump animations
    for (auto it = m_bumpAnims.begin(); it != m_bumpAnims.end(); ) {
        it->timer--;

        // Bump goes up for 8 frames, then back down for 8 frames
        if (it->timer > 8) {
            it->offsetY = -4.0f * (16 - it->timer) / 8.0f;  // Rising
        } else {
            it->offsetY = -4.0f * it->timer / 8.0f;  // Falling back
        }

        if (it->timer <= 0) {
            it = m_bumpAnims.erase(it);
        } else {
            ++it;
        }
    }
}

int Tilemap::getQuestionAnimFrame() const {
    // 3-frame shimmer animation, cycles every 24 game frames
    int phase = (m_globalAnimTimer / 8) % 3;
    return phase;
}

int Tilemap::getTileFrame(TileType type) const {
    // Maps tile types to frame indices in the tileset sprite sheet.
    // The sprite sheet is organized in a grid, and each tile type
    // maps to a specific frame index.
    //
    // This mapping will be finalized when we create the actual tileset,
    // but for now we define a logical ordering.
    switch (type) {
        case TileType::EMPTY:             return -1;  // Don't render
        case TileType::GROUND:            return 0;
        case TileType::BRICK:
        case TileType::BRICK_COINS:
        case TileType::BRICK_STAR:        return 1;  // All bricks look identical
        case TileType::QUESTION_COIN:
        case TileType::QUESTION_MUSHROOM:
        case TileType::QUESTION_STAR:
        case TileType::QUESTION_1UP:      return 2 + getQuestionAnimFrame();  // Animated: frames 2,3,4
        case TileType::USED_BLOCK:        return 5;
        case TileType::HIDDEN_COIN:
        case TileType::HIDDEN_1UP:        return -1;  // Invisible until hit
        case TileType::PIPE_TOP_LEFT:     return 6;
        case TileType::PIPE_TOP_RIGHT:    return 7;
        case TileType::PIPE_BODY_LEFT:    return 8;
        case TileType::PIPE_BODY_RIGHT:   return 9;
        case TileType::FLAGPOLE_TOP:      return 10;
        case TileType::FLAGPOLE_SHAFT:    return 11;
        case TileType::CASTLE_BLOCK:      return 12;
        case TileType::CASTLE_BATTLEMENT: return 13;
        case TileType::CASTLE_DOOR:       return 14;
        case TileType::CASTLE_WINDOW:     return 15;
        case TileType::STAIR_BLOCK:       return 16;
        case TileType::COIN:              return 17;  // TODO: animate

        // Background decorations
        case TileType::CLOUD_LEFT:        return 18;
        case TileType::CLOUD_MID:         return 19;
        case TileType::CLOUD_RIGHT:       return 20;
        case TileType::CLOUD_BOTTOM_LEFT: return 21;
        case TileType::CLOUD_BOTTOM_MID:  return 22;
        case TileType::CLOUD_BOTTOM_RIGHT:return 23;
        case TileType::BUSH_LEFT:         return 24;
        case TileType::BUSH_MID:          return 25;
        case TileType::BUSH_RIGHT:        return 26;
        case TileType::HILL_TOP:          return 27;
        case TileType::HILL_LEFT:         return 28;
        case TileType::HILL_RIGHT:        return 29;
        case TileType::HILL_FILL:         return 30;
        case TileType::HILL_SPOT:         return 31;

        default: return -1;
    }
}
