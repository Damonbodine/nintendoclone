#include "physics/Collision.h"
#include "world/Tilemap.h"
#include "core/Constants.h"
#include <cmath>
#include <algorithm>

namespace Collision {

void worldToTile(float worldX, float worldY, int& tileX, int& tileY) {
    tileX = static_cast<int>(std::floor(worldX / Constants::TILE_SIZE));
    tileY = static_cast<int>(std::floor(worldY / Constants::TILE_SIZE));
}

TileCollisionResult resolveWithTilemap(
    const AABB& box,
    float velX, float velY,
    const Tilemap& tilemap)
{
    TileCollisionResult result;
    float newX = box.x;
    float newY = box.y;

    // --- Resolve horizontal axis first ---
    newX += velX;

    AABB hBox = { newX, box.y, box.w, box.h };

    // Check tiles that the box now overlaps
    int startTileX, startTileY, endTileX, endTileY;
    worldToTile(hBox.left(), hBox.top(), startTileX, startTileY);
    worldToTile(hBox.right() - 0.001f, hBox.bottom() - 0.001f, endTileX, endTileY);

    for (int ty = startTileY; ty <= endTileY; ty++) {
        for (int tx = startTileX; tx <= endTileX; tx++) {
            if (tilemap.isSolid(tx, ty)) {
                AABB tileBox = {
                    static_cast<float>(tx * Constants::TILE_SIZE),
                    static_cast<float>(ty * Constants::TILE_SIZE),
                    static_cast<float>(Constants::TILE_SIZE),
                    static_cast<float>(Constants::TILE_SIZE)
                };

                if (hBox.overlaps(tileBox)) {
                    if (velX > 0) {
                        // Moving right — push left
                        newX = tileBox.left() - box.w;
                        result.hitRight = true;
                    } else if (velX < 0) {
                        // Moving left — push right
                        newX = tileBox.right();
                        result.hitLeft = true;
                    }
                    result.hitTileX = tx;
                    result.hitTileY = ty;

                    // Rebuild hBox after resolution
                    hBox = { newX, box.y, box.w, box.h };
                }
            }
        }
    }

    // --- Resolve vertical axis ---
    newY += velY;

    AABB vBox = { newX, newY, box.w, box.h };

    worldToTile(vBox.left(), vBox.top(), startTileX, startTileY);
    worldToTile(vBox.right() - 0.001f, vBox.bottom() - 0.001f, endTileX, endTileY);

    for (int ty = startTileY; ty <= endTileY; ty++) {
        for (int tx = startTileX; tx <= endTileX; tx++) {
            if (tilemap.isSolid(tx, ty)) {
                AABB tileBox = {
                    static_cast<float>(tx * Constants::TILE_SIZE),
                    static_cast<float>(ty * Constants::TILE_SIZE),
                    static_cast<float>(Constants::TILE_SIZE),
                    static_cast<float>(Constants::TILE_SIZE)
                };

                if (vBox.overlaps(tileBox)) {
                    if (velY > 0) {
                        // Moving down — land on top
                        newY = tileBox.top() - box.h;
                        result.hitBottom = true;
                    } else if (velY < 0) {
                        // Moving up — hit ceiling
                        newY = tileBox.bottom();
                        result.hitTop = true;
                    }
                    result.hitTileX = tx;
                    result.hitTileY = ty;

                    // Rebuild vBox after resolution
                    vBox = { newX, newY, box.w, box.h };
                }
            }
        }
    }

    result.resolvedX = newX;
    result.resolvedY = newY;
    return result;
}

bool isPointInSolidTile(float x, float y, const Tilemap& tilemap) {
    int tx, ty;
    worldToTile(x, y, tx, ty);
    return tilemap.isSolid(tx, ty);
}

bool isOverlappingSolid(const AABB& box, const Tilemap& tilemap) {
    int startTileX, startTileY, endTileX, endTileY;
    worldToTile(box.left(), box.top(), startTileX, startTileY);
    worldToTile(box.right() - 0.001f, box.bottom() - 0.001f, endTileX, endTileY);

    for (int ty = startTileY; ty <= endTileY; ty++) {
        for (int tx = startTileX; tx <= endTileX; tx++) {
            if (tilemap.isSolid(tx, ty)) {
                return true;
            }
        }
    }
    return false;
}

bool entitiesOverlap(const AABB& a, const AABB& b) {
    return a.overlaps(b);
}

bool isStomping(const AABB& attacker, const AABB& target, float attackerVelY) {
    // Attacker must be moving downward
    if (attackerVelY <= 0) return false;

    // Attacker's bottom must be in the top portion of the target.
    // Use generous tolerance — at high fall speeds Mario can penetrate
    // several pixels per frame, and tilemap collision may have shifted him.
    float targetMidY = target.top() + target.h * 0.6f;
    return attacker.bottom() <= targetMidY + 6.0f &&
           attacker.bottom() >= target.top() - 4.0f;
}

}  // namespace Collision
