#include "Tower.h"
#include <cmath>

Tower::Tower(int x, int y) : x(x), y(y) {}

void Tower::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 100, 100, 255, 255);
    SDL_Rect rect = { x - 10, y - 10, 20, 20 };
    SDL_RenderFillRect(renderer, &rect);
}

bool Tower::isInRange(const Enemy& enemy) {
    int dx = x - enemy.getX();
    int dy = y - enemy.getY();
    return (dx*dx + dy*dy) <= (range * range);
}

void Tower::shoot(Enemy& target) {
    target.takeDamage(1); // le baja 1 de vida
}

void Tower::update(std::vector<Enemy>& enemies) {
    if (fireCooldown > 0) {
        fireCooldown--;
        return;
    }

    for (Enemy& e : enemies) {
        if (isInRange(e)) {
            shoot(e);
            fireCooldown = fireRate;
            break; // dispara solo a uno por ciclo
        }
    }
}
