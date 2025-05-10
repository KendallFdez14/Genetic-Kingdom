#include "Enemy.h"
#include <SDL2/SDL.h>
#include <cmath>
#include <random>

// Clase base
Enemy::Enemy(int x, int y, const std::vector<std::pair<int, int>>& path, int health, float speed, float arrowRes, float magicRes, float artilleryRes, SDL_Texture* texture, int gold)
    : x(x), y(y), path(path), health(health), speed(speed), arrowResistance(arrowRes), magicResistance(magicRes), artilleryResistance(artilleryRes), texture(texture), gold(gold) {}

void Enemy::update() {
    if (currentNode >= path.size()) return; // Si ya llegó al final del camino

    int targetX = path[currentNode].first * 75 + 37; // Centro de la celda
    int targetY = path[currentNode].second * 75 + 37;

    float dx = targetX - x;
    float dy = targetY - y;
    float dist = std::sqrt(dx * dx + dy * dy);

    if (dist <= speed) {
        // Si llegó al nodo actual, pasa al siguiente
        x = targetX;
        y = targetY;
        currentNode++;
    } else {
        // Movimiento hacia el nodo actual
        x += (dx / dist) * speed;
        y += (dy / dist) * speed;
    }
}

void Enemy::render(SDL_Renderer* renderer) {
    SDL_Rect rect = { x - 16, y - 16, 32, 32 }; // Tamaño del sprite
    SDL_RenderCopy(renderer, texture, nullptr, &rect); // Renderizar la textura
}

void Enemy::takeDamage(int dmg, const std::string& type) {
    if (type == "arrow") {
        health -= static_cast<int>(dmg * (1.0f - arrowResistance));
    } else if (type == "magic") {
        health -= static_cast<int>(dmg * (1.0f - magicResistance));
    } else if (type == "artillery") {
        health -= static_cast<int>(dmg * (1.0f - artilleryResistance));
    }
}

bool Enemy::isDead() const {
    return health <= 0;
}

int Enemy::getX() const { return x; }
int Enemy::getY() const { return y; }
int Enemy::getGold() const { return gold; }
std::vector<std::pair<int, int>> Enemy::getPath() const { return path; }
SDL_Texture* Enemy::getTexture() const { return texture; }

float Enemy::calculateFitness() const {
    float resistance = hitsTaken / pathLength;
    float time = timeTaken / pathLength;
    float distance = distanceTraveled / pathLength;
    return resistance * time * distance;
}

// Implementación de la mutación
void Enemy::mutate(float mutationRate) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    
    // Mutación de velocidad
    if (dis(gen) < mutationRate) {
        float speedChange = dis(gen) * 0.4f + 0.8f; // Factor entre 0.8 y 1.2
        speed *= speedChange;
        // Limitar velocidad a un rango razonable
        speed = std::max(1.0f, std::min(5.0f, speed));
    }
    
    // Mutación de salud
    if (dis(gen) < mutationRate) {
        float healthChange = dis(gen) * 0.4f + 0.8f; // Factor entre 0.8 y 1.2
        health = static_cast<int>(health * healthChange);
        // Limitar salud a un rango razonable
        health = std::max(50, std::min(300, health));
    }
    
    // Mutación de resistencias
    auto mutateResistance = [&](float& resistance) {
        if (dis(gen) < mutationRate) {
            float change = dis(gen) * 0.3f - 0.15f; // -0.15 a +0.15
            resistance += change;
            resistance = std::max(0.0f, std::min(0.9f, resistance)); // Limitar entre 0 y 0.9
        }
    };
    
    mutateResistance(arrowResistance);
    mutateResistance(magicResistance);
    mutateResistance(artilleryResistance);
    
    // Mutación de oro
    if (dis(gen) < mutationRate) {
        float goldChange = dis(gen) * 0.4f + 0.8f; // Factor entre 0.8 y 1.2
        gold = static_cast<int>(gold * goldChange);
        gold = std::max(5, gold); // Al menos 5 de oro
    }
}

// Implementación del cruce
Enemy* Enemy::crossover(const Enemy* parent1, const Enemy* parent2,
                       const std::vector<std::pair<int, int>>& path,
                       SDL_Texture* texture) {
    // Randomly select crossover point
    int crossPoint = rand() % 5;
    
    // New attributes for child
    int newHealth;
    float newSpeed;
    float newArrowRes;
    float newMagicRes;
    float newArtilleryRes;
    int newGold;
    
    // Apply crossover based on the crossover point
    newHealth = (crossPoint >= 0) ? parent2->getHealth() : parent1->getHealth();
    newSpeed = (crossPoint >= 1) ? parent2->getSpeed() : parent1->getSpeed();
    newArrowRes = (crossPoint >= 2) ? parent2->getArrowResistance() : parent1->getArrowResistance();
    newMagicRes = (crossPoint >= 3) ? parent2->getMagicResistance() : parent1->getMagicResistance();
    newArtilleryRes = (crossPoint >= 4) ? parent2->getArtilleryResistance() : parent1->getArtilleryResistance();
    newGold = (parent1->getGold() + parent2->getGold()) / 2;
    
    // Determine which type of enemy to create based on the first parent
    Enemy* child = nullptr;
    
    if (dynamic_cast<const Ogre*>(parent1)) {
        child = new Ogre(0, 0, path, texture);
    } else if (dynamic_cast<const DarkElf*>(parent1)) {
        child = new DarkElf(0, 0, path, texture);
    } else if (dynamic_cast<const Harpy*>(parent1)) {
        child = new Harpy(0, 0, path, texture);
    } else if (dynamic_cast<const Mercenary*>(parent1)) {
        child = new Mercenary(0, 0, path, texture);
    } else {
        // Default to Ogre if type cannot be determined
        child = new Ogre(0, 0, path, texture);
    }
    
    // Apply the new attributes
    child->setHealth(newHealth);
    child->setSpeed(newSpeed);
    child->setArrowResistance(newArrowRes);
    child->setMagicResistance(newMagicRes);
    child->setArtilleryResistance(newArtilleryRes);
    child->setGold(newGold);
    
    return child;
}

// Ogro
Ogre::Ogre(int x, int y, const std::vector<std::pair<int, int>>& path, SDL_Texture* texture)
    : Enemy(x, y, path, 150, 1.5f, 0.5f, 0.2f, 0.2f, texture, 15) {}

Enemy* Ogre::clone() const {
    return new Ogre(x, y, path, texture);
}

// Elfo Oscuro
DarkElf::DarkElf(int x, int y, const std::vector<std::pair<int, int>>& path, SDL_Texture* texture)
    : Enemy(x, y, path, 100, 3.5f, 0.2f, 0.5f, 0.2f, texture, 10) {}

Enemy* DarkElf::clone() const {
    return new DarkElf(x, y, path, texture);
}

// Harpía
Harpy::Harpy(int x, int y, const std::vector<std::pair<int, int>>& path, SDL_Texture* texture)
    : Enemy(x, y, path, 120, 2.5f, 0.3f, 0.3f, 1.0f, texture, 12) {}

Enemy* Harpy::clone() const {
    return new Harpy(x, y, path, texture);
}

// Mercenario
Mercenary::Mercenary(int x, int y, const std::vector<std::pair<int, int>>& path, SDL_Texture* texture)
    : Enemy(x, y, path, 110, 2.0f, 0.4f, 0.1f, 0.4f, texture, 11) {}

Enemy* Mercenary::clone() const {
    return new Mercenary(x, y, path, texture);
}
