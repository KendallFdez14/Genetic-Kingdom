#include "Enemy.h"
#include <SDL2/SDL.h>
#include <cmath>

Enemy::Enemy(int x, int y, const std::vector<std::pair<int, int>>& path)
    : x(x), y(y), path(path) {}

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
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect rect = { x - 8, y - 8, 16, 16 };
    SDL_RenderFillRect(renderer, &rect);
}

bool Enemy::isDead() const {
    return health <= 0;
}

void Enemy::takeDamage(int dmg) {
    health -= dmg;
}

int Enemy::getX() const { return x; }
int Enemy::getY() const { return y; }
