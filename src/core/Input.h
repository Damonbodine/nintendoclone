#pragma once

#include <SDL2/SDL.h>

// Maps keyboard/gamepad input to NES-style button states.
// Tracks both current state and edge detection (just pressed/released).
class Input {
public:
    // NES controller buttons
    enum Button {
        LEFT = 0,
        RIGHT,
        UP,
        DOWN,
        A,       // Jump
        B,       // Run / Fireball
        START,
        SELECT,
        BUTTON_COUNT
    };

    Input();

    // Call once per frame before processing events
    void beginFrame();

    // Process an SDL event
    void handleEvent(const SDL_Event& event);

    // Button state queries
    bool isHeld(Button btn) const;
    bool isPressed(Button btn) const;   // True only on the frame the button was pressed
    bool isReleased(Button btn) const;  // True only on the frame the button was released

private:
    bool m_current[BUTTON_COUNT] = {};
    bool m_previous[BUTTON_COUNT] = {};

    void setButtonState(SDL_Scancode key, bool pressed);
};
