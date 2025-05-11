#include "Enemy.h"
#include <SDL2/SDL.h>
#include <cmath>

// Clase base
Enemy::Enemy(int x, int y, const std::vector<std::pair<int, int>>& path, int health, float speed, float arrowRes, float magicRes, float artilleryRes, SDL_Texture* texture, int gold)
    : x(x), y(y), path(path), health(health), speed(speed), arrowResistance(arrowRes), magicResistance(magicRes), artilleryResistance(artilleryRes), texture(texture), gold(gold) {}

void Enemy::update() {
    if (currentNode >= path.size()) return; // Si ya llegó al final del camino

    // Coordenadas del nodo objetivo
    int targetX = path[currentNode].first * 75 + 37; // Centro de la celda
    int targetY = path[currentNode].second * 75 + 37;

    // Calcular la distancia al nodo objetivo
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

void Enemy::takeDamage(int damage, const std::string& type) {
    if (type == "arrow") {
        health -= static_cast<int>(damage * (1.0f - arrowResistance));
    } else if (type == "magic") {
        health -= static_cast<int>(damage * (1.0f - magicResistance));
    } else if (type == "artillery") {
        health -= static_cast<int>(damage * (1.0f - artilleryResistance));
    } else if (type == "special") {
        health -= damage; // Daño especial sin resistencias
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
