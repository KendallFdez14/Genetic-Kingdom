#pragma once

#include <vector>
#include <memory>
#include "Enemy.h"
#include "GeneticAlgorithm.h"
#include <SDL2/SDL.h>

class WaveManager {
private:
    std::vector<std::pair<int, int>> path;
    SDL_Texture* ogreTexture;
    SDL_Texture* darkElfTexture;
    SDL_Texture* harpyTexture;
    SDL_Texture* mercenaryTexture;
    std::vector<std::unique_ptr<Enemy>> currentWave;
    std::vector<std::unique_ptr<Enemy>> readyEnemies;
    std::vector<std::unique_ptr<Enemy>> survivors;
    std::vector<std::unique_ptr<Enemy>> processedEnemies; // Enemies that died or reached the end
    int currentWaveNumber;
    bool waveActive = false;
    int spawnTimer = 0;
    int waveTimer = 0;
    GeneticAlgorithm geneticAlgorithm;

public:
    WaveManager(const std::vector<std::pair<int, int>>& path, SDL_Texture* ogreTexture, SDL_Texture* darkElfTexture, SDL_Texture* harpyTexture, SDL_Texture* mercenaryTexture);
    void update();
    void startNewWave();
    std::vector<std::unique_ptr<Enemy>> generateInitialWave();
    void saveSurvivors(const std::vector<std::unique_ptr<Enemy>>& survivors);
    std::vector<std::unique_ptr<Enemy>> getCurrentWave();
    bool isWaveComplete();
    int getCurrentWaveNumber();
    bool isWaveActive();
    
    // New methods for genetic algorithm
    void addProcessedEnemies(std::vector<std::unique_ptr<Enemy>> deadEnemies);
    void prepareNextWave();
}; 