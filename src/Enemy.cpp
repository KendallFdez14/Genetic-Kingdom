#include "Enemy.h"
#include <SDL2/SDL.h>
#include <cmath>
#include <random>

// Clase base
Enemy::Enemy(int x, int y, const std::vector<std::pair<int, int>>& path, int health, float speed, float arrowRes, float magicRes, float artilleryRes, SDL_Texture* texture, int gold)
    : x(x), y(y), path(path), health(health), initialHealth(health), speed(speed), arrowResistance(arrowRes), magicResistance(magicRes), artilleryResistance(artilleryRes), texture(texture), gold(gold) {}

void Enemy::update() {
    if (currentNode >= path.size()) return; // Si ya llegó al final del camino

<<<<<<< HEAD
    // Track time even when not moving
    timeTaken += 1.0f; // Assuming update is called once per frame at fixed interval
    
=======
    // Coordenadas del nodo objetivo
>>>>>>> Develop
    int targetX = path[currentNode].first * 75 + 37; // Centro de la celda
    int targetY = path[currentNode].second * 75 + 37;

    // Calcular la distancia al nodo objetivo
    float dx = targetX - x;
    float dy = targetY - y;
    float dist = std::sqrt(dx * dx + dy * dy);

    if (dist <= speed) {
        // Track distance traveled when reaching a node
        distanceTraveled += dist;
        
        // Si llegó al nodo actual, pasa al siguiente
        x = targetX;
        y = targetY;
        currentNode++;
        
        // Update path length when moving to a new node
        if (currentNode == 1) {
            // Initialize path length based on total number of nodes
            pathLength = path.size();
        }
    } else {
        // Track distance traveled during movement
        float moveDist = speed;
        distanceTraveled += moveDist;
        
        // Movimiento hacia el nodo actual
        x += (dx / dist) * speed;
        y += (dy / dist) * speed;
    }
}

void Enemy::render(SDL_Renderer* renderer) {
    SDL_Rect rect = { x - 16, y - 16, 32, 32 }; // Tamaño del sprite
    SDL_RenderCopy(renderer, texture, nullptr, &rect); // Renderizar la textura
}

<<<<<<< HEAD
void Enemy::takeDamage(int dmg, const std::string& type) {
    int actualDamage = 0;
    
    if (type == "arrow") {
        actualDamage = static_cast<int>(dmg * (1.0f - arrowResistance));
    } else if (type == "magic") {
        actualDamage = static_cast<int>(dmg * (1.0f - magicResistance));
    } else if (type == "artillery") {
        actualDamage = static_cast<int>(dmg * (1.0f - artilleryResistance));
    }
    
    health -= actualDamage;
    
    // If any damage was taken, increment hitsTaken counter
    if (actualDamage > 0) {
        hitsTaken++;
=======
void Enemy::takeDamage(int damage, const std::string& type) {
    if (type == "arrow") {
        health -= static_cast<int>(damage * (1.0f - arrowResistance));
    } else if (type == "magic") {
        health -= static_cast<int>(damage * (1.0f - magicResistance));
    } else if (type == "artillery") {
        health -= static_cast<int>(damage * (1.0f - artilleryResistance));
    } else if (type == "special") {
        health -= damage; // Daño especial sin resistencias
>>>>>>> Develop
    }
}

bool Enemy::isDead() const {
    return health <= 0;
}

bool Enemy::hasReachedEnd() const {
    return currentNode >= path.size(); // Si el enemigo ha pasado el último nodo
}

int Enemy::getX() const { return x; }
int Enemy::getY() const { return y; }
int Enemy::getGold() const { return gold; }
std::vector<std::pair<int, int>> Enemy::getPath() const { return path; }
SDL_Texture* Enemy::getTexture() const { return texture; }

float Enemy::calculateFitness() const {
    // Base survival score - reaching the end is the primary goal
    float survivalScore = hasReachedEnd() ? 10.0f : (distanceTraveled / pathLength);
    
    // Damage absorption capacity - normalized between 0 and 1
    // Original health vs damage taken
    float healthRatio = health > 0 ? static_cast<float>(health) / initialHealth : 0.0f;
    
    // Time efficiency - faster enemies get higher scores
    // This is inverted since lower time is better
    float timeEfficiency = pathLength > 0 ? 1.0f / (timeTaken / pathLength + 0.1f) : 0.0f;
    
    // Resource efficiency - better enemies cost more gold
    // We want enemies that are worth their cost
    float goldEfficiency = static_cast<float>(gold) / 50.0f; // Normalize by max gold value
    
    // Resistance factors - weight resistances based on tower frequency in game
    float resistanceScore = (arrowResistance * 0.4f + 
                           magicResistance * 0.3f + 
                           artilleryResistance * 0.3f);
    
    // Type-specific bonus to encourage diversity
    float typeBonus = 1.0f;
    if (dynamic_cast<const Ogre*>(this)) typeBonus = 1.1f;  // Slightly favor ogres
    else if (dynamic_cast<const DarkElf*>(this)) typeBonus = 1.05f;
    else if (dynamic_cast<const Harpy*>(this)) typeBonus = 1.15f;  // Favor harpies more
    else if (dynamic_cast<const Mercenary*>(this)) typeBonus = 1.0f;
    
    // Combined fitness score with appropriate weights
    float fitness = (survivalScore * 3.0f) +      // Survival is most important
                   (healthRatio * 1.5f) +         // Health remaining
                   (timeEfficiency * 2.0f) +      // Speed is very important
                   (goldEfficiency * 1.0f) +      // Gold value matters
                   (resistanceScore * 2.0f);      // Resistances are important
    
    // Apply type bonus
    fitness *= typeBonus;
    
    // Apply damage absorption bonus - enemies that absorbed lots of damage without dying
    if (!isDead() && hitsTaken > 0) {
        fitness *= (1.0f + (hitsTaken / 20.0f)); // Bonus for absorbing hits
    }
    
    return fitness;
}

// Implementación de la mutación
void Enemy::mutate(float mutationRate) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    
    // Mutation of speed - more aggressive changes
    if (dis(gen) < mutationRate) {
        float speedChange = dis(gen) * 0.6f + 0.7f; // Factor between 0.7 and 1.3
        speed *= speedChange;
        // Limit speed to a reasonable range, but allow higher values
        speed = std::max(1.0f, std::min(6.0f, speed));
    }
    
    // Mutation of health - more aggressive changes
    if (dis(gen) < mutationRate) {
        float healthChange = dis(gen) * 0.6f + 0.7f; // Factor between 0.7 and 1.3
        health = static_cast<int>(health * healthChange);
        // Wider health range
        health = std::max(50, std::min(400, health));
        initialHealth = health; // Update initial health too
    }
    
    // Mutation of resistances - more significant changes
    auto mutateResistance = [&](float& resistance) {
        if (dis(gen) < mutationRate) {
            float change = dis(gen) * 0.4f - 0.2f; // -0.2 to +0.2
            resistance += change;
            resistance = std::max(0.0f, std::min(0.9f, resistance)); // Limit between 0 and 0.9
        }
    };
    
    mutateResistance(arrowResistance);
    mutateResistance(magicResistance);
    mutateResistance(artilleryResistance);
    
    // Mutation of gold - more significant changes
    if (dis(gen) < mutationRate) {
        float goldChange = dis(gen) * 0.6f + 0.7f; // Factor between 0.7 and 1.3
        gold = static_cast<int>(gold * goldChange);
        gold = std::max(5, std::min(30, gold)); // Allow higher gold values
    }

    // Reset position and node
    x = 0;
    y = 0;
    currentNode = 0;
    
    // Reset fitness tracking
    hitsTaken = 0;
    timeTaken = 0;
    distanceTraveled = 0;
    pathLength = 0;
}

Enemy* Enemy::clone() const {
    return new Enemy(0, 0, path, health, speed, arrowResistance, magicResistance, artilleryResistance, texture, gold);
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
    
    child = new Enemy(0, 0, path, newHealth, newSpeed, newArrowRes, newMagicRes, newArtilleryRes, texture, newGold);
    
    return child;
}

// Ogro
Ogre::Ogre(int x, int y, const std::vector<std::pair<int, int>>& path, SDL_Texture* texture)
    : Enemy(x, y, path, 150, 1.5f, 0.5f, 0.2f, 0.2f, texture, 15) {}


// Elfo Oscuro
DarkElf::DarkElf(int x, int y, const std::vector<std::pair<int, int>>& path, SDL_Texture* texture)
    : Enemy(x, y, path, 100, 3.5f, 0.2f, 0.5f, 0.2f, texture, 10) {}


// Harpía
Harpy::Harpy(int x, int y, const std::vector<std::pair<int, int>>& path, SDL_Texture* texture)
    : Enemy(x, y, path, 120, 2.5f, 0.3f, 0.3f, 1.0f, texture, 12) {}


// Mercenario
Mercenary::Mercenary(int x, int y, const std::vector<std::pair<int, int>>& path, SDL_Texture* texture)
    : Enemy(x, y, path, 110, 2.0f, 0.4f, 0.1f, 0.4f, texture, 11) {}

