#pragma once
#include "Enemy.h"
#include <vector>
#include <memory>

class GeneticAlgorithm {
    public:
        GeneticAlgorithm();
        std::vector<std::unique_ptr<Enemy>> evolvePopulation(const std::vector<std::unique_ptr<Enemy>>& previousWave);

    private:
        std::vector<std::unique_ptr<Enemy>> selectParents(const std::vector<std::unique_ptr<Enemy>>& population);
        std::vector<std::unique_ptr<Enemy>> crossover(const std::unique_ptr<Enemy>& parent1, const std::unique_ptr<Enemy>& parent2);
        std::vector<std::unique_ptr<Enemy>> createNextGeneration(const std::vector<std::unique_ptr<Enemy>>& parents, const std::vector<std::unique_ptr<Enemy>>& offspring);
        void mutate(std::unique_ptr<Enemy>& enemy, float mutationRate);
};