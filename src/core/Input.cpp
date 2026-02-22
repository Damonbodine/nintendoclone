#include "core/Input.h"

Input::Input() {
    for (int i = 0; i < BUTTON_COUNT; i++) {
        m_current[i] = false;
        m_previous[i] = false;
    }
}

void Input::beginFrame() {
    for (int i = 0; i < BUTTON_COUNT; i++) {
        m_previous[i] = m_current[i];
    }
}

void Input::handleEvent(const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
        bool pressed = (event.type == SDL_KEYDOWN);
        setButtonState(event.key.keysym.scancode, pressed);
    }
}

bool Input::isHeld(Button btn) const {
    return m_current[btn];
}

bool Input::isPressed(Button btn) const {
    return m_current[btn] && !m_previous[btn];
}

bool Input::isReleased(Button btn) const {
    return !m_current[btn] && m_previous[btn];
}

void Input::setButtonState(SDL_Scancode key, bool pressed) {
    switch (key) {
        // Arrow keys
        case SDL_SCANCODE_LEFT:  m_current[LEFT] = pressed; break;
        case SDL_SCANCODE_RIGHT: m_current[RIGHT] = pressed; break;
        case SDL_SCANCODE_UP:    m_current[UP] = pressed; break;
        case SDL_SCANCODE_DOWN:  m_current[DOWN] = pressed; break;

        // WASD alternative
        case SDL_SCANCODE_A:     m_current[LEFT] = pressed; break;
        case SDL_SCANCODE_D:     m_current[RIGHT] = pressed; break;
        case SDL_SCANCODE_W:     m_current[UP] = pressed; break;
        case SDL_SCANCODE_S:     m_current[DOWN] = pressed; break;

        // Action buttons
        case SDL_SCANCODE_Z:
        case SDL_SCANCODE_SPACE: m_current[A] = pressed; break;  // Jump
        case SDL_SCANCODE_X:
        case SDL_SCANCODE_LSHIFT: m_current[B] = pressed; break; // Run/Fire
        case SDL_SCANCODE_RETURN: m_current[START] = pressed; break;
        case SDL_SCANCODE_RSHIFT: m_current[SELECT] = pressed; break;

        default: break;
    }
}
