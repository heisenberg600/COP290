#include <iostream>
#include <fstream>
#include "Game.cpp"
using namespace std;

int main(int argc, char const *argv[])
{
    Game *game = new Game();
    game->init("Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500, 600);

    while (game->isOn()) {
        game->eventManager();
        game->update();
        game->render();
    }

    game->clean();
    return 0;
}


