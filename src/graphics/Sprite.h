#pragma once

#include <SDL2/SDL.h>
#include "graphics/Animation.h"
#include <unordered_map>
#include <string>

// A Sprite manages multiple named animations and renders the active one.
class Sprite {
public:
    Sprite();

    // Add a named animation
    void addAnimation(const std::string& name, const Animation& anim);

    // Set the active animation (resets it if different from current)
    void setAnimation(const std::string& name);

    // Get current animation name
    const std::string& getCurrentAnimationName() const { return m_currentName; }

    // Update the current animation
    void update();

    // Render at the given NES-coordinate position
    void render(SDL_Renderer* renderer, int x, int y, bool flipH = false);

    // Get the current animation (for querying frame info)
    Animation* getCurrentAnimation();

    // Set visibility (for flicker effects)
    void setVisible(bool visible) { m_visible = visible; }
    bool isVisible() const { return m_visible; }

private:
    std::unordered_map<std::string, Animation> m_animations;
    std::string m_currentName;
    Animation* m_current = nullptr;
    bool m_visible = true;
};
