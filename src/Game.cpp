#include "Game.h"
#include <iostream>
#include <algorithm>
#include <SDL2/SDL_image.h> // Include SDL_image for IMG_LoadTexture
#include <SDL2/SDL_ttf.h> // Include SDL_ttf for TTF_RenderText_Blended
#include <sstream>
#include <iomanip> // For std::fixed and std::setprecision
#include <numeric>
#include "Tower.h"
#include "Astar.h"
#include "WaveManager.h"

SDL_Texture* ogreTexture = nullptr;
SDL_Texture* darkElfTexture = nullptr;
SDL_Texture* harpyTexture = nullptr;
SDL_Texture* mercenaryTexture = nullptr;
TTF_Font* font = nullptr;

Game::Game() : window(nullptr), renderer(nullptr), running(false), enemyTimer(0), gold(50), selectedTowerType(TowerType::Archer), waveManager(std::vector<std::pair<int, int>>(), nullptr, nullptr, nullptr, nullptr) {
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

    window = SDL_CreateWindow("Tower Defense", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, mapSize * 75 + 500, mapSize * 75, 0);
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

    // Initialize WaveManager with the loaded textures
    waveManager = WaveManager(path, ogreTexture, darkElfTexture, harpyTexture, mercenaryTexture);

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
        if (event.type == SDL_QUIT) {
            running = false;
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
        towers.emplace_back(col * 75 + 37, row * 75 + 37, selectedTowerType, mapSize);
        map[row][col] = 2; // Marca la celda como ocupada
        gold -= Tower::getCost(selectedTowerType);
    }
}

void Game::update() {
    waveTimer++;
    enemyTimer++;
    
    // Update wave manager every frame to handle enemy spawning
    waveManager.update();
    
    // Get any ready enemies and add them to the game
    auto waveEnemies = waveManager.getCurrentWave();
    if (!waveEnemies.empty()) {
        for (auto& enemy : waveEnemies) {
            if (enemy) {  // Make sure we have a valid enemy
                enemies.push_back(std::move(enemy));
            }
        }
    }
    
    // Process all active enemies using WaveManager
    // This will handle updates, deaths, and enemies that reached the end
    auto [processedEnemies, goldEarned] = waveManager.processActiveEnemies(enemies);
    
    // Add any gold earned to the player's total
    gold += goldEarned;
    
    // Check if a wave transition has occurred and handle it
    bool noEnemiesLeft = enemies.empty();
    waveManager.checkAndHandleWaveTransition(noEnemiesLeft);
    
    // Update towers
    for (auto& tower : towers) {
        tower.update(enemies, projectiles);
    }
    
    // Update projectiles
    for (auto it = projectiles.begin(); it != projectiles.end();) {
        if (!(*it) || (*it)->hasHit()) {  // Check for null pointer
            it = projectiles.erase(it);
        } else {
            (*it)->update();
            ++it;
        }
    }
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderClear(renderer);

    renderMap(); // Renderizar el mapa
    renderPannel();
    for (auto& tower : towers)
        tower.render(renderer);

        for (auto& enemy : enemies)
        enemy->render(renderer); // Llamar a render() en el puntero

    for (auto& projectile : projectiles)
        projectile->render(renderer); // Llamar a render() en el puntero

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


void Game::renderPannel() {
    //Background
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect pannel = {mapSize * 75, 0, 500, mapSize * 75};
    SDL_RenderFillRect(renderer, &pannel);

    //padding (10)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_Rect pannelRect = {mapSize * 75 + 10, 10, 480, mapSize * 75 - 20};
    SDL_RenderDrawRect(renderer, &pannelRect);

    int yOffset = 10;
    // Render text
    SDL_Color color = {0, 0, 0};
    
    // Gold Text
    std::stringstream ss;
    ss << "Gold: " << gold;
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, ss.str().c_str(), color);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect textRect = {mapSize * 75 + 15, yOffset, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    
    yOffset += 30;
    
    // Wave Text
    ss.str("");
    ss << "Wave: " << waveManager.getCurrentWaveNumber();
    textSurface = TTF_RenderText_Blended(font, ss.str().c_str(), color);
    textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    textRect = {mapSize * 75 + 15, yOffset, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    
    yOffset += 30;
    
    // Get fitness stats
    const FitnessStats& stats = waveManager.getLastWaveFitness();
    
    // Only show evolution details if we have at least one generation
    if (stats.generation > 0) {
        // Title for evolutionary stats
        ss.str("");
        ss << "Evolution Stats:";
        textSurface = TTF_RenderText_Blended(font, ss.str().c_str(), color);
        textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        textRect = {mapSize * 75 + 15, yOffset, textSurface->w, textSurface->h};
        SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
        
        yOffset += 25;
        
        // Generation
        ss.str("");
        ss << "Generation: " << stats.generation;
        textSurface = TTF_RenderText_Blended(font, ss.str().c_str(), color);
        textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        textRect = {mapSize * 75 + 15, yOffset, textSurface->w, textSurface->h};
        SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
        
        yOffset += 25;
        
        // Mutation Rate
        ss.str("");
        ss << "Mutation: " << std::fixed << std::setprecision(1) << (stats.mutationRate * 100) << "%";
        textSurface = TTF_RenderText_Blended(font, ss.str().c_str(), color);
        textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        textRect = {mapSize * 75 + 15, yOffset, textSurface->w, textSurface->h};
        SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
        
        yOffset += 25;
        
        // Average Fitness
        ss.str("");
        ss << "Avg Fitness: " << std::fixed << std::setprecision(1) << stats.averageFitness;
        textSurface = TTF_RenderText_Blended(font, ss.str().c_str(), color);
        textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        textRect = {mapSize * 75 + 15, yOffset, textSurface->w, textSurface->h};
        SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
        
        yOffset += 25;
        
        // Best Fitness
        ss.str("");
        ss << "Best Fitness: " << std::fixed << std::setprecision(1) << stats.maxFitness;
        textSurface = TTF_RenderText_Blended(font, ss.str().c_str(), color);
        textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        textRect = {mapSize * 75 + 15, yOffset, textSurface->w, textSurface->h};
        SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }
    
    // Tower selection section
    yOffset += 40;
    
    ss.str("");
    ss << "Towers:";
    textSurface = TTF_RenderText_Blended(font, ss.str().c_str(), color);
    textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    textRect = {mapSize * 75 + 15, yOffset, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    
    yOffset += 25;
    
    // Tower types and costs
    ss.str("");
    ss << "1: Archer (" << Tower::getCost(TowerType::Archer) << " gold)";
    textSurface = TTF_RenderText_Blended(font, ss.str().c_str(), color);
    textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    textRect = {mapSize * 75 + 15, yOffset, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    
    yOffset += 25;
    
    ss.str("");
    ss << "2: Mage (" << Tower::getCost(TowerType::Mage) << " gold)";
    textSurface = TTF_RenderText_Blended(font, ss.str().c_str(), color);
    textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    textRect = {mapSize * 75 + 15, yOffset, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    
    yOffset += 25;
    
    ss.str("");
    ss << "3: Artillery (" << Tower::getCost(TowerType::Artillery) << " gold)";
    textSurface = TTF_RenderText_Blended(font, ss.str().c_str(), color);
    textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    textRect = {mapSize * 75 + 15, yOffset, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    
    // Highlight selected tower type
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 128);
    int selectedY = 0;
    switch (selectedTowerType) {
        case TowerType::Archer:
            selectedY = yOffset - 50;
            break;
        case TowerType::Mage:
            selectedY = yOffset - 25;
            break;
        case TowerType::Artillery:
            selectedY = yOffset;
            break;
    }
    
    SDL_Rect highlightRect = {mapSize * 75 + 12, selectedY, 176, 25};
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderFillRect(renderer, &highlightRect);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
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
