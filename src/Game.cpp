#include "Game.h"
#include <iostream>
#include <algorithm>
#include <SDL2/SDL_image.h> // Include SDL_image for IMG_LoadTexture
#include <SDL2/SDL_ttf.h> // Include SDL_ttf for TTF_RenderText_Blended
#include <sstream>
#include <numeric>
#include "Tower.h"
#include "Astar.h"

SDL_Texture* ogreTexture = nullptr;
SDL_Texture* darkElfTexture = nullptr;
SDL_Texture* harpyTexture = nullptr;
SDL_Texture* mercenaryTexture = nullptr;
TTF_Font* font = nullptr;

Game::Game() : window(nullptr), renderer(nullptr), running(false), enemyTimer(0), gold(50), selectedTowerType(TowerType::Archer), enemiesDefeated(0), enemiesReachedEnd(0) {
    // Inicializar el mapa 16x16 con un camino
    map = {
        {1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0},
        {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
        {0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0},
        {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
        {0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0}
    };
    mapSize = map.size();

    // Definir el camino como una lista de nodos
    path = aStarSearch(map, {0, 0}, {15, 14}); // Llamar a la función A* para obtener el camino
}

Game::~Game() {
    clean();
}

bool Game::init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init error: " << SDL_GetError() << "\n";
        return false;
    }

    window = SDL_CreateWindow("Tower Defense", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, mapSize * 75 + 200, mapSize * 75, 0);
    if (!window) {
        std::cerr << "SDL_CreateWindow error: " << SDL_GetError() << "\n";
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer error: " << SDL_GetError() << "\n";
        return false;
    }

    // Cargar texturas
    ogreTexture = IMG_LoadTexture(renderer, "../assets/ogre.png");
    darkElfTexture = IMG_LoadTexture(renderer, "../assets/dark_elf.png");
    harpyTexture = IMG_LoadTexture(renderer, "../assets/harpy.png");
    mercenaryTexture = IMG_LoadTexture(renderer, "../assets/mercenary.png");

    if (!ogreTexture || !darkElfTexture || !harpyTexture || !mercenaryTexture) {
        std::cerr << "Error al cargar texturas: " << SDL_GetError() << "\n";
        return false;
    }

    //Initialize SDL_ttf
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf error: " << TTF_GetError() << "\n";
        return false;
    }

    font = TTF_OpenFont("../assets/BigBlueT.ttf", 24);
    if (!font) {
        std::cerr << "SDL_ttf error: " << TTF_GetError() << "\n";
        return false;
    }

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

    // Mostrar pantalla de derrota si el jugador pierde
    if (enemiesReachedEnd >= 5) {
        renderDefeatScreen();
    }
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        } else if (event.type == SDL_MOUSEMOTION) {
            mouseX = event.motion.x;
            mouseY = event.motion.y;
        } else if (event.type == SDL_MOUSEBUTTONDOWN) {
            if (event.button.button == SDL_BUTTON_LEFT) {
                placeTower(event.button.x, event.button.y);
            }
        } else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_1:
                    selectedTowerType = TowerType::Archer;
                    break;
                case SDLK_2:
                    selectedTowerType = TowerType::Mage;
                    break;
                case SDLK_3:
                    selectedTowerType = TowerType::Artillery;
                    break;
                case SDLK_w:
                    // Intentar mejorar la torreta seleccionada
                    for (auto& tower : towers) {
                        if (tower->isMouseOver(mouseX, mouseY)) { // Verificar si el mouse está sobre la torreta
                            if (tower->upgrade(gold)) {
                                std::cout << "Torreta mejorada al nivel " << tower->getLevel() << "\n";
                            } else {
                                std::cout << "No se pudo mejorar la torreta.\n";
                            }
                            break;
                        }
                    }
                    break;
            }
        }
    }
}

void Game::placeTower(int mouseX, int mouseY) {
    int col = mouseX / 75;
    int row = mouseY / 75;

    if (gold < Tower::getCost(selectedTowerType)) {
        // No se puede comprar la torre
        return;
    }

    if (row >= 0 && row < mapSize && col >= 0 && col < mapSize && map[row][col] == 0) {
        // Coloca una torre del tipo seleccionado en la celda
        towers.emplace_back(std::make_unique<Tower>(col * 75 + 37, row * 75 + 37, selectedTowerType));
        map[row][col] = 2; // Marca la celda como ocupada
        gold -= Tower::getCost(selectedTowerType);
    }
}

void Game::update() {
    enemyTimer++;
    if (enemyTimer >= 120) { // Cada 2 segundos
        spawnEnemy();
        enemyTimer = 0;
    }

    // Actualizar enemigos
    for (auto it = enemies.begin(); it != enemies.end();) {
        (*it)->update();

        // Verificar si el enemigo llegó al final del camino
        if ((*it)->hasReachedEnd()) {
            enemiesReachedEnd++; // Incrementar el contador
            it = enemies.erase(it); // Eliminar enemigo de la lista

            // Verificar condición de derrota
            if (enemiesReachedEnd >= 5) {
                running = false; // Detener el juego
                return;
            }
        } else {
            ++it;
        }
    }

    // Actualizar torretas
    for (auto& tower : towers) {
        tower->update(enemies, projectiles);
    }

    // Actualizar proyectiles
    for (auto it = projectiles.begin(); it != projectiles.end();) {
        (*it)->update();
        if ((*it)->hasHit()) {
            it = projectiles.erase(it); // Eliminar proyectil si ha impactado
        } else {
            ++it;
        }
    }

    // Incrementar el contador de enemigos eliminados y acumular oro
    for (auto it = enemies.begin(); it != enemies.end();) {
        if ((*it)->isDead()) {
            gold += (*it)->getGold(); // Acumular oro del enemigo eliminado
            enemiesDefeated++;       // Incrementar el contador de enemigos eliminados
            it = enemies.erase(it);  // Eliminar enemigo de la lista
        } else {
            ++it;
        }
    }
}

void Game::render() {
    // Renderizar el mapa
    renderMap();

    // Renderizar las torretas
    for (auto& tower : towers) {
        bool isSelected = tower->isMouseOver(mouseX, mouseY); // Verificar si el mouse está sobre la torreta
        tower->render(renderer, isSelected);
    }

    // Renderizar enemigos y proyectiles
    for (auto& enemy : enemies) {
        enemy->render(renderer);
    }
    for (auto& projectile : projectiles) {
        projectile->render(renderer);
    }

    // Renderizar el panel lateral (incluye el oro)
    renderPannel();

    SDL_RenderPresent(renderer);
}

void Game::renderMap() {
    int cellSize = 75; // Tamaño de cada celda (800px / 8 = 100px por celda)
    for (int row = 0; row < mapSize; ++row) {
        for (int col = 0; col < mapSize; ++col) {
            if (map[row][col] == 1) {
                SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255); // Color del camino
            } else {
                SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); // Color del espacio vacío
            }
            SDL_Rect cell = {col * cellSize, row * cellSize, cellSize, cellSize};
            SDL_RenderFillRect(renderer, &cell);
        }
    }
}

void Game::spawnEnemy() {
// Alternar entre diferentes tipos de enemigos
    static int enemyType = 0;

    switch (enemyType) {
        case 0:
            enemies.emplace_back(std::make_unique<Ogre>(0, 0, path, ogreTexture));
            break;
        case 1:
            enemies.emplace_back(std::make_unique<DarkElf>(0, 0, path, darkElfTexture));
            break;
        case 2:
            enemies.emplace_back(std::make_unique<Harpy>(0, 0, path, harpyTexture));
            break;
        case 3:
            enemies.emplace_back(std::make_unique<Mercenary>(0, 0, path, mercenaryTexture));
            break;
    }

    enemyType = (enemyType + 1) % 4; // Alternar entre los tipos de enemigos
}

void Game::renderPannel() {
    // Fondo del panel
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Blanco
    SDL_Rect pannel = {mapSize * 75, 0, 250, mapSize * 75}; // Ancho del panel ajustado a 250
    SDL_RenderFillRect(renderer, &pannel);

    // Borde del panel
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Negro
    SDL_Rect pannelRect = {mapSize * 75 + 10, 10, 230, mapSize * 75 - 20};
    SDL_RenderDrawRect(renderer, &pannelRect);

    // Texto del oro
    SDL_Color color = {0, 0, 0}; // Negro
    std::stringstream ss;
    ss << "Gold: " << gold;

    SDL_Surface* textSurface = TTF_RenderText_Blended_Wrapped(font, ss.str().c_str(), color, 220); // Ajustar ancho del texto
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect textRect = {mapSize * 75 + 20, 20, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);

    // Mostrar la cantidad de enemigos eliminados
    std::stringstream defeatedInfo;
    defeatedInfo << "Enemies Defeated: " << enemiesDefeated;

    SDL_Surface* defeatedSurface = TTF_RenderText_Blended_Wrapped(font, defeatedInfo.str().c_str(), color, 220);
    SDL_Texture* defeatedTexture = SDL_CreateTextureFromSurface(renderer, defeatedSurface);
    SDL_Rect defeatedRect = {mapSize * 75 + 20, 50, defeatedSurface->w, defeatedSurface->h};
    SDL_RenderCopy(renderer, defeatedTexture, nullptr, &defeatedRect);
    SDL_FreeSurface(defeatedSurface);
    SDL_DestroyTexture(defeatedTexture);

    // Mostrar el nivel y tipo de torre si el mouse está sobre una torreta
    for (auto& tower : towers) {
        if (tower->isMouseOver(mouseX, mouseY)) {
            std::stringstream towerInfo;
            towerInfo << "Tower: " << (tower->getType() == TowerType::Archer ? "Archer" :
                                        tower->getType() == TowerType::Mage ? "Mage" : "Artillery");
            towerInfo << " (Level " << tower->getLevel() << ")";

            SDL_Surface* towerSurface = TTF_RenderText_Blended_Wrapped(font, towerInfo.str().c_str(), color, 220);
            SDL_Texture* towerTexture = SDL_CreateTextureFromSurface(renderer, towerSurface);
            SDL_Rect towerRect = {mapSize * 75 + 20, 100, towerSurface->w, towerSurface->h}; // Cambiar posición a 100
            SDL_RenderCopy(renderer, towerTexture, nullptr, &towerRect);
            SDL_FreeSurface(towerSurface);
            SDL_DestroyTexture(towerTexture);
            break;
        }
    }
}

void Game::renderDefeatScreen() {
    // Limpiar la pantalla
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Negro
    SDL_RenderClear(renderer);

    // Mostrar mensaje de derrota
    SDL_Color color = {255, 0, 0}; // Rojo
    std::string message = "You Lost! Press ESC to Exit.";
    SDL_Surface* surface = TTF_RenderText_Blended(font, message.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect rect = {mapSize * 75 / 2 - surface->w / 2, mapSize * 75 / 2 - surface->h / 2, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, nullptr, &rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

    SDL_RenderPresent(renderer);

    // Esperar a que el jugador presione ESC
    SDL_Event event;
    while (true) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                return;
            }
        }
    }
}

void Game::clean() {
    // Liberar texturas
    if (ogreTexture) SDL_DestroyTexture(ogreTexture);
    if (darkElfTexture) SDL_DestroyTexture(darkElfTexture);
    if (harpyTexture) SDL_DestroyTexture(harpyTexture);
    if (mercenaryTexture) SDL_DestroyTexture(mercenaryTexture);

    // Liberar el renderer y la ventana
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);

    SDL_Quit();
}
