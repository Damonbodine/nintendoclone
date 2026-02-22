#pragma once

#include <SDL2/SDL.h>

class Game;

// Abstract base class for game states (title, playing, pause, etc.)
class GameState {
public:
    virtual ~GameState() = default;

    virtual void enter() = 0;
    virtual void exit() = 0;
    virtual void update(Game& game) = 0;
    virtual void render(SDL_Renderer* renderer) = 0;
};
