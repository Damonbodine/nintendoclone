#include "graphics/SpriteSheet.h"

SpriteSheet::SpriteSheet() {}

void SpriteSheet::init(SDL_Texture* texture, int frameWidth, int frameHeight) {
    m_texture = texture;
    m_frameWidth = frameWidth;
    m_frameHeight = frameHeight;

    SDL_QueryTexture(texture, nullptr, nullptr, &m_textureWidth, &m_textureHeight);
    m_columns = m_textureWidth / frameWidth;
    m_rows = m_textureHeight / frameHeight;
}

SDL_Rect SpriteSheet::getFrameRect(int index) const {
    int col = index % m_columns;
    int row = index / m_columns;
    return { col * m_frameWidth, row * m_frameHeight, m_frameWidth, m_frameHeight };
}

SDL_Rect SpriteSheet::getFrameRect(int col, int row) const {
    return { col * m_frameWidth, row * m_frameHeight, m_frameWidth, m_frameHeight };
}
