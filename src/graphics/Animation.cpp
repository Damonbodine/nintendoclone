#include "graphics/Animation.h"

Animation::Animation() {}

void Animation::init(SpriteSheet* sheet, const std::vector<AnimationFrame>& frames, bool loops) {
    m_sheet = sheet;
    m_frames = frames;
    m_loops = loops;
    m_currentFrame = 0;
    m_frameTimer = 0;
    m_finished = false;
}

void Animation::update() {
    if (m_finished || m_frames.empty()) return;

    m_frameTimer++;

    if (m_frameTimer >= m_frames[m_currentFrame].durationFrames) {
        m_frameTimer = 0;
        m_currentFrame++;

        if (m_currentFrame >= static_cast<int>(m_frames.size())) {
            if (m_loops) {
                m_currentFrame = 0;
            } else {
                m_currentFrame = static_cast<int>(m_frames.size()) - 1;
                m_finished = true;
            }
        }
    }
}

void Animation::reset() {
    m_currentFrame = 0;
    m_frameTimer = 0;
    m_finished = false;
}

SDL_Rect Animation::getCurrentFrameRect() const {
    if (m_frames.empty() || !m_sheet) {
        return { 0, 0, 0, 0 };
    }
    return m_sheet->getFrameRect(m_frames[m_currentFrame].frameIndex);
}

SDL_Texture* Animation::getTexture() const {
    return m_sheet ? m_sheet->getTexture() : nullptr;
}

void Animation::setFrameDuration(int duration) {
    for (auto& frame : m_frames) {
        frame.durationFrames = duration;
    }
}
