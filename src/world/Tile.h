#pragma once

#include <cstdint>

// Tile types that make up the game world
enum class TileType : uint8_t {
    EMPTY = 0,           // Sky / passable
    GROUND,              // Solid brown ground
    BRICK,               // Breakable brick block
    QUESTION_COIN,       // ? block containing a coin
    QUESTION_MUSHROOM,   // ? block containing mushroom/fire flower
    QUESTION_STAR,       // ? block containing star
    QUESTION_1UP,        // ? block containing 1-UP mushroom
    USED_BLOCK,          // Spent ? block (brown, solid)
    HIDDEN_COIN,         // Invisible block with coin
    HIDDEN_1UP,          // Invisible block with 1-UP
    PIPE_TOP_LEFT,       // Pipe top-left segment
    PIPE_TOP_RIGHT,      // Pipe top-right segment
    PIPE_BODY_LEFT,      // Pipe body-left segment
    PIPE_BODY_RIGHT,     // Pipe body-right segment
    FLAGPOLE_TOP,        // Flagpole ball on top
    FLAGPOLE_SHAFT,      // Flagpole shaft
    CASTLE_BLOCK,        // Castle wall block
    CASTLE_BATTLEMENT,   // Castle top with crenellations
    CASTLE_DOOR,         // Castle entrance
    CASTLE_WINDOW,       // Castle window
    STAIR_BLOCK,         // Staircase block (solid, brown)
    COIN,                // Floating coin (collectible, not solid)

    // Background decorations (not solid, rendered behind main layer)
    CLOUD_LEFT,
    CLOUD_MID,
    CLOUD_RIGHT,
    CLOUD_BOTTOM_LEFT,
    CLOUD_BOTTOM_MID,
    CLOUD_BOTTOM_RIGHT,
    BUSH_LEFT,
    BUSH_MID,
    BUSH_RIGHT,
    HILL_TOP,
    HILL_LEFT,
    HILL_RIGHT,
    HILL_FILL,
    HILL_SPOT,           // Spots on the hill

    TILE_TYPE_COUNT
};

// Properties of a tile
namespace TileProperties {

    inline bool isSolid(TileType type) {
        switch (type) {
            case TileType::GROUND:
            case TileType::BRICK:
            case TileType::QUESTION_COIN:
            case TileType::QUESTION_MUSHROOM:
            case TileType::QUESTION_STAR:
            case TileType::QUESTION_1UP:
            case TileType::USED_BLOCK:
            case TileType::PIPE_TOP_LEFT:
            case TileType::PIPE_TOP_RIGHT:
            case TileType::PIPE_BODY_LEFT:
            case TileType::PIPE_BODY_RIGHT:
            case TileType::CASTLE_BLOCK:
            case TileType::CASTLE_BATTLEMENT:
            case TileType::STAIR_BLOCK:
                return true;
            default:
                return false;
        }
    }

    inline bool isQuestion(TileType type) {
        return type == TileType::QUESTION_COIN ||
               type == TileType::QUESTION_MUSHROOM ||
               type == TileType::QUESTION_STAR ||
               type == TileType::QUESTION_1UP;
    }

    inline bool isHidden(TileType type) {
        return type == TileType::HIDDEN_COIN ||
               type == TileType::HIDDEN_1UP;
    }

    inline bool isBrick(TileType type) {
        return type == TileType::BRICK;
    }

    inline bool isPipe(TileType type) {
        return type == TileType::PIPE_TOP_LEFT ||
               type == TileType::PIPE_TOP_RIGHT ||
               type == TileType::PIPE_BODY_LEFT ||
               type == TileType::PIPE_BODY_RIGHT;
    }

    inline bool isFlagpole(TileType type) {
        return type == TileType::FLAGPOLE_TOP ||
               type == TileType::FLAGPOLE_SHAFT;
    }

    inline bool isBackground(TileType type) {
        return type >= TileType::CLOUD_LEFT && type <= TileType::HILL_SPOT;
    }

    inline bool isInteractive(TileType type) {
        return isQuestion(type) || isBrick(type) || isHidden(type);
    }
}
