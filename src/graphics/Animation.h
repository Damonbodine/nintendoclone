#pragma once

#include <SDL2/SDL.h>
#include <vector>
#include "graphics/SpriteSheet.h"

// An animation is a sequence of frames from a sprite sheet,
// each displayed for a configurable duration.
struct AnimationFrame {
    int frameIndex;       // Index into the sprite sheet
    int durationFrames;   // How many game frames this frame is displayed
};

class Animation {
public:
    Animation();

    // Set up the animation with a sprite sheet and frame sequence
    void init(SpriteSheet* sheet, const std::vector<AnimationFrame>& frames, bool loops = true);

    // Update the animation by one game frame
    void update();

    // Reset to the first frame
    void reset();

    // Get the current frame's source rectangle from the sprite sheet
    SDL_Rect getCurrentFrameRect() const;

    // Get the sprite sheet texture
    SDL_Texture* getTexture() const;

    // Is the animation finished (only relevant for non-looping animations)
    bool isFinished() const { return m_finished; }

    // Override frame duration dynamically (for speed-dependent walk animation)
    void setFrameDuration(int duration);

    // Get current frame index in the sequence
    int getCurrentFrameIndex() const { return m_currentFrame; }

private:
    SpriteSheet* m_sheet = nullptr;
    std::vector<AnimationFrame> m_frames;
    int m_currentFrame = 0;
    int m_frameTimer = 0;
    bool m_loops = true;
    bool m_finished = false;
};
