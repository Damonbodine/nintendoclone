#include "core/Game.h"
#include "states/TitleState.h"
#include <cstdio>
#include <memory>

int main(int /* argc */, char* /* argv */[]) {
    Game game;

    if (!game.init()) {
        std::fprintf(stderr, "Game initialization failed!\n");
        return 1;
    }

    // Start with the title screen
    game.changeState(std::make_unique<TitleState>(game));

    game.run();
    game.shutdown();

    return 0;
}
