#include "Enemy.h"
#include <SDL2/SDL.h>
#include <cmath>

// Clase base
Enemy::Enemy(int x, int y, const std::vector<std::pair<int, int>>& path, int health, float speed, float arrowRes, float magicRes, float artilleryRes, SDL_Texture* texture)
    : x(x), y(y), path(path), health(health), speed(speed), arrowResistance(arrowRes), magicResistance(magicRes), artilleryResistance(artilleryRes), texture(texture) {}

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

// Ogro
Ogre::Ogre(int x, int y, const std::vector<std::pair<int, int>>& path, SDL_Texture* texture)
    : Enemy(x, y, path, 150, 1.5f, 0.5f, 0.2f, 0.2f, texture) {}

// Elfo Oscuro
DarkElf::DarkElf(int x, int y, const std::vector<std::pair<int, int>>& path, SDL_Texture* texture)
    : Enemy(x, y, path, 100, 3.5f, 0.2f, 0.5f, 0.2f, texture) {}

// Harpía
Harpy::Harpy(int x, int y, const std::vector<std::pair<int, int>>& path, SDL_Texture* texture)
    : Enemy(x, y, path, 120, 2.5f, 0.3f, 0.3f, 1.0f, texture) {}

// Mercenario
Mercenary::Mercenary(int x, int y, const std::vector<std::pair<int, int>>& path, SDL_Texture* texture)
    : Enemy(x, y, path, 110, 2.0f, 0.4f, 0.1f, 0.4f, texture) {}
