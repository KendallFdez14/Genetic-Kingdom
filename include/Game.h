#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include "Enemy.h"
#include "Tower.h"

class Game {
public:
    Game();
    ~Game();
    bool init();
    void run();
    void clean();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;

    std::vector<Enemy> enemies;
    std::vector<Tower> towers;

    void handleEvents();
    void update();
    void render();
    void spawnEnemy();
    int enemyTimer;
};
