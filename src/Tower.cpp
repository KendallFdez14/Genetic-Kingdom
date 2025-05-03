#include "Tower.h"
#include <cmath>

Tower::Tower(int x, int y, TowerType type) : x(x), y(y), type(type) {}

void Tower::render(SDL_Renderer* renderer) {
    // Elegir color por tipo
    switch (type) {
        case TowerType::Archer:
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Verde
            break;
        case TowerType::Mage:
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Azul
            break;
        case TowerType::Artillery:
            SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255); // Naranja
            break;
    }

    SDL_Rect rect = { x - 10, y - 10, 20, 20 };
    SDL_RenderFillRect(renderer, &rect);
}

bool Tower::isInRange(const Enemy& enemy) {
    int dx = x - enemy.getX();
    int dy = y - enemy.getY();
    return (dx*dx + dy*dy) <= (range * range);
}

void Tower::shoot(Enemy& target, std::vector<Projectile>& projectiles) {
    projectiles.emplace_back(x, y, &target);
}

void Tower::update(std::vector<Enemy>& enemies, std::vector<Projectile>& projectiles) {
    if (fireCooldown > 0) {
        fireCooldown--;
        return;
    }

    for (Enemy& e : enemies) {
        if (isInRange(e)) {
            shoot(e, projectiles);
            fireCooldown = fireRate;
            break;
        }
    }
}
