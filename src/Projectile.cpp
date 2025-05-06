#include "Projectile.h"
#include <SDL2/SDL.h>
#include <cmath>
Projectile::Projectile(int x, int y, Enemy* target, int damage)
    : x(x), y(y), target(target), damage(damage) {}

void Projectile::update() {
    if (!target || target->isDead()) {
        hit = true;
        return;
    }

    float dx = target->getX() - x;
    float dy = target->getY() - y;
    float dist = std::sqrt(dx*dx + dy*dy);

    if (dist < 5.0f) {
        target->takeDamage(damage);
        hit = true;
        return;
    }

    x += (dx / dist) * speed;
    y += (dy / dist) * speed;
}
bool Projectile::hasHit() const {
    return hit;
}

void Projectile::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Color del proyectil
    SDL_Rect rect = { static_cast<int>(x) - 2, static_cast<int>(y) - 2, 4, 4 };
    SDL_RenderFillRect(renderer, &rect);
}


