#include "Tower.h"
#include <cmath>

Tower::Tower(int x, int y, TowerType type)
    : x(x), y(y), type(type) {
    configureStats();
    currentSpecialCooldown = specialCooldown;
}

void Tower::configureStats() {
    switch (type) {
        case TowerType::Archer:
            damage = 5;
            range = 180;
            fireRate = 30;
            specialCooldown = 600;
            break;
        case TowerType::Mage:
            damage = 10;
            range = 140;
            fireRate = 60;
            specialCooldown = 900;
            break;
        case TowerType::Artillery:
            damage = 20;
            range = 200;
            fireRate = 120;
            specialCooldown = 1200;
            break;
    }
}

bool Tower::isInRange(const Enemy& enemy) {
    int dx = x - enemy.getX();
    int dy = y - enemy.getY();
    return (dx * dx + dy * dy) <= (range * range);
}

void Tower::shoot(Enemy& target, std::vector<Projectile>& projectiles) {
    Projectile p(x, y, &target, damage);
    projectiles.push_back(p);
}

void Tower::update(std::vector<Enemy>& enemies, std::vector<Projectile>& projectiles) {
    if (fireCooldown > 0) fireCooldown--;
    if (currentSpecialCooldown > 0) currentSpecialCooldown--;

    if (fireCooldown == 0) {
        for (Enemy& e : enemies) {
            if (isInRange(e)) {
                shoot(e, projectiles);
                fireCooldown = fireRate;
                break;
            }
        }
    }
}

void Tower::render(SDL_Renderer* renderer) {
    switch (type) {
        case TowerType::Archer: SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); break;
        case TowerType::Mage: SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); break;
        case TowerType::Artillery: SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255); break;
    }
    SDL_Rect rect = { x - 10, y - 10, 20, 20 };
    SDL_RenderFillRect(renderer, &rect);
}
