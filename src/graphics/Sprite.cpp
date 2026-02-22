#include "graphics/Sprite.h"

Sprite::Sprite() {}

void Sprite::addAnimation(const std::string& name, const Animation& anim) {
    m_animations[name] = anim;
}

void Sprite::setAnimation(const std::string& name) {
    if (name == m_currentName) return;

    auto it = m_animations.find(name);
    if (it != m_animations.end()) {
        m_currentName = name;
        m_current = &it->second;
        m_current->reset();
    }
}

void Sprite::update() {
    if (m_current) {
        m_current->update();
    }
}

void Sprite::render(SDL_Renderer* renderer, int x, int y, bool flipH) {
    if (!m_visible || !m_current) return;

    SDL_Texture* texture = m_current->getTexture();
    if (!texture) return;

    SDL_Rect src = m_current->getCurrentFrameRect();
    SDL_Rect dst = { x, y, src.w, src.h };

    SDL_RendererFlip flip = flipH ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    SDL_RenderCopyEx(renderer, texture, &src, &dst, 0.0, nullptr, flip);
}

Animation* Sprite::getCurrentAnimation() {
    return m_current;
}
