#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <memory>
#include "Tower.h"
#include "Enemy.h"
#include "Projectile.h"
#include "Astar.h"
#include "WaveManager.h"
class Game {
public:
    Game();
    ~Game();
    bool init();
    void run();
    void clean();
    void renderDefeatScreen();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;

    std::vector<std::vector<int>> map; // Representación del mapa
    std::vector<std::pair<int, int>> path; // Lista de nodos del camino
    int enemyTimer;
    int waveTimer;
    float timeScale = 1.0f; // Time scale factor for debugging
    int mouseX = 0;
    int mouseY = 0;
    
    int enemiesDefeated = 0;
    int enemiesReachedEnd = 0;

    std::vector<std::unique_ptr<Tower>> towers; // Changed to store unique_ptr
    std::vector<std::unique_ptr<Enemy>> enemies;
    std::vector<std::unique_ptr<Projectile>> projectiles;

    TowerType selectedTowerType; // Tipo de torre seleccionado

    void handleEvents();
    void update();
    void render();

    void renderMap();
    void renderPannel();
    void placeTower(int mouseX, int mouseY);

    int gold;
    int mapSize;

    WaveManager waveManager;
};
