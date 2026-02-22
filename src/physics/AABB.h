#pragma once

// Axis-Aligned Bounding Box for collision detection.
// All values in pixels (float for sub-pixel precision).
struct AABB {
    float x, y;     // Top-left corner
    float w, h;     // Width and height

    float left() const   { return x; }
    float right() const  { return x + w; }
    float top() const    { return y; }
    float bottom() const { return y + h; }

    float centerX() const { return x + w / 2.0f; }
    float centerY() const { return y + h / 2.0f; }

    // Test if this AABB overlaps another
    bool overlaps(const AABB& other) const {
        return left() < other.right() &&
               right() > other.left() &&
               top() < other.bottom() &&
               bottom() > other.top();
    }

    // Get overlap depth on each axis (positive means overlapping)
    float overlapX(const AABB& other) const {
        float overlapLeft = right() - other.left();
        float overlapRight = other.right() - left();
        return (overlapLeft < overlapRight) ? overlapLeft : -overlapRight;
    }

    float overlapY(const AABB& other) const {
        float overlapTop = bottom() - other.top();
        float overlapBottom = other.bottom() - top();
        return (overlapTop < overlapBottom) ? overlapTop : -overlapBottom;
    }
};
