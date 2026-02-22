#pragma once

#include <SDL2/SDL.h>
#include <string>
#include <vector>

// A sprite sheet is a single texture containing multiple sprite frames
// arranged in a grid or at specific positions.
class SpriteSheet {
public:
    SpriteSheet();
    ~SpriteSheet() = default;

    // Initialize with a loaded SDL texture and its frame layout
    void init(SDL_Texture* texture, int frameWidth, int frameHeight);

    // Get the source rectangle for a frame by index (row-major order)
    SDL_Rect getFrameRect(int index) const;

    // Get the source rectangle for a frame by column/row
    SDL_Rect getFrameRect(int col, int row) const;

    SDL_Texture* getTexture() const { return m_texture; }
    int getFrameWidth() const { return m_frameWidth; }
    int getFrameHeight() const { return m_frameHeight; }
    int getColumns() const { return m_columns; }
    int getRows() const { return m_rows; }

private:
    SDL_Texture* m_texture = nullptr;
    int m_frameWidth = 0;
    int m_frameHeight = 0;
    int m_columns = 0;
    int m_rows = 0;
    int m_textureWidth = 0;
    int m_textureHeight = 0;
};
