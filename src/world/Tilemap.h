#pragma once

#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include <unordered_map>
#include "world/Tile.h"

class Renderer;
class SpriteSheet;

// The Tilemap is a 2D grid of tiles representing the game world.
// Handles rendering, collision queries, and tile modification (breaking bricks, etc.)
class Tilemap {
public:
    Tilemap();
    ~Tilemap() = default;

    // Load level data from file
    bool loadFromFile(const std::string& path);

    // Initialize with dimensions (for programmatic level building)
    void init(int width, int height);

    // Set/get tiles
    void setTile(int x, int y, TileType type);
    TileType getTile(int x, int y) const;

    // Collision query
    bool isSolid(int tileX, int tileY) const;

    // Render visible tiles given a camera offset
    void render(SDL_Renderer* renderer, SpriteSheet* tileset, float cameraX, float cameraY);

    // Render background decorations
    void renderBackground(SDL_Renderer* renderer, SpriteSheet* tileset, float cameraX, float cameraY);

    // Dimensions
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    int getPixelWidth() const { return m_width * 16; }
    int getPixelHeight() const { return m_height * 16; }

    // Interactive tile events
    struct BlockHitResult {
        TileType originalType = TileType::EMPTY;
        int tileX = 0;
        int tileY = 0;
        bool spawnsItem = false;
        bool breaksBrick = false;
    };

    // Called when Mario hits a block from below
    BlockHitResult hitBlockFromBelow(int tileX, int tileY, bool isBigMario);

    // Block bump animation state
    struct BumpAnimation {
        int tileX, tileY;
        int timer;         // Frames remaining
        float offsetY;     // Current Y offset (negative = up)
    };

    void updateAnimations();
    const std::vector<BumpAnimation>& getBumpAnimations() const { return m_bumpAnims; }

    // ? block shimmer animation (global timer)
    int getQuestionAnimFrame() const;

private:
    int m_width = 0;
    int m_height = 0;
    std::vector<TileType> m_tiles;        // Main tile layer
    std::vector<TileType> m_background;   // Background decoration layer

    std::vector<BumpAnimation> m_bumpAnims;
    int m_globalAnimTimer = 0;  // For ? block shimmer

    // Multi-coin brick tracking: maps tile index -> coins remaining
    std::unordered_map<int, int> m_coinBrickCounters;
    std::unordered_map<int, int> m_coinBrickTimers;  // Countdown timer per brick

    int tileIndex(int x, int y) const { return y * m_width + x; }
    bool inBounds(int x, int y) const { return x >= 0 && x < m_width && y >= 0 && y < m_height; }

    // Get the sprite sheet frame index for a given tile type
    int getTileFrame(TileType type) const;
};
