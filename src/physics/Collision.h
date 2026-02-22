#pragma once

#include "physics/AABB.h"
#include <vector>

class Tilemap;

// Collision detection and response for the tile-based world.
namespace Collision {

// Result of a tile collision check
struct TileCollisionResult {
    bool hitLeft = false;
    bool hitRight = false;
    bool hitTop = false;     // Hit ceiling / block from below
    bool hitBottom = false;  // Landed on ground

    int hitTileX = -1;  // Tile coordinate of the most recent hit
    int hitTileY = -1;

    float resolvedX = 0;  // Position after collision resolution
    float resolvedY = 0;
};

// Check and resolve collision between an AABB and the tilemap.
// Resolves horizontal and vertical axes independently.
// Returns collision info including which sides were hit and the resolved position.
TileCollisionResult resolveWithTilemap(
    const AABB& box,
    float velX, float velY,
    const Tilemap& tilemap
);

// Check if a point is inside a solid tile
bool isPointInSolidTile(float x, float y, const Tilemap& tilemap);

// Check if an AABB overlaps any solid tile
bool isOverlappingSolid(const AABB& box, const Tilemap& tilemap);

// Get the tile coordinates for a point
void worldToTile(float worldX, float worldY, int& tileX, int& tileY);

// Entity vs Entity collision (simple overlap check)
bool entitiesOverlap(const AABB& a, const AABB& b);

// Check if entity A is stomping entity B (A's bottom hits B's top half)
bool isStomping(const AABB& attacker, const AABB& target, float attackerVelY);

}  // namespace Collision
