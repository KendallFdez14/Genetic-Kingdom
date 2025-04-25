#include "Game.h"
#include <iostream>
#include <algorithm>

Game::Game() : window(nullptr), renderer(nullptr), running(false), enemyTimer(0) {}

Game::~Game() {
    clean();
}

bool Game::init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init error: " << SDL_GetError() << "\n";
        return false;
    }

    window = SDL_CreateWindow("Tower Defense", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);
    if (!window) return false;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) return false;

    // Torre inicial
    towers.emplace_back(200, 300);

    running = true;
    return true;
}

void Game::run() {
    while (running) {
        handleEvents();
        update();
        render();
        SDL_Delay(16); // ~60 FPS
    }
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT)
            running = false;
    }
}

void Game::update() {
    enemyTimer++;
    if (enemyTimer >= 120) { // Cada 2 segundos
        spawnEnemy();
        enemyTimer = 0;
    }

    for (auto& tower : towers)
    tower.update(enemies);
    
    for (auto& enemy : enemies)
        enemy.update();
        
    enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
        [](Enemy& e) { return e.isDead(); }), enemies.end());
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderClear(renderer);

    for (auto& tower : towers)
        tower.render(renderer);

    for (auto& enemy : enemies)
        enemy.render(renderer);

    SDL_RenderPresent(renderer);
}

void Game::spawnEnemy() {
    enemies.emplace_back(0, 320);
}

void Game::clean() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
