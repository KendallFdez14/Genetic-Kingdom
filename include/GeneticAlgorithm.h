#pragma once
#include "Enemy.h"
#include <vector>
#include <memory>

class GeneticAlgorithm {
    public:
        GeneticAlgorithm();
        std::vector<std::unique_ptr<Enemy>> evolvePopulation(const std::vector<std::unique_ptr<Enemy>>& previousWave);
        
        // Get current mutation rate
        float getMutationRate() const { return adaptiveMutationRate; }
        
        // Get current generation number
        int getGeneration() const { return generation; }

    private:
        std::vector<std::unique_ptr<Enemy>> selectParents(const std::vector<std::unique_ptr<Enemy>>& population);
        std::vector<std::unique_ptr<Enemy>> crossover(const std::unique_ptr<Enemy>& parent1, const std::unique_ptr<Enemy>& parent2);
        std::vector<std::unique_ptr<Enemy>> createNextGeneration(const std::vector<std::unique_ptr<Enemy>>& parents, const std::vector<std::unique_ptr<Enemy>>& offspring);
        void mutate(std::unique_ptr<Enemy>& enemy, float mutationRate);
        
        // Create random enemies for immigration
        std::vector<std::unique_ptr<Enemy>> createRandomEnemies(int count, const std::vector<std::pair<int, int>>& path, SDL_Texture* ogreTexture, SDL_Texture* darkElfTexture, SDL_Texture* harpyTexture, SDL_Texture* mercenaryTexture);
        
        // Update the mutation rate based on fitness improvement
        void updateMutationRate(const std::vector<std::unique_ptr<Enemy>>& population);
        
        // Adaptive mutation parameters
        float baseMutationRate = 0.05f;
        float adaptiveMutationRate = 0.05f;
        int stagnantGenerations = 0;
        float previousBestFitness = 0.0f;
        int generation = 0;
        
        // Immigration parameters
        int immigrationInterval = 3; // Every 3 generations
        float immigrationRate = 0.25f; // Replace 25% of population
};