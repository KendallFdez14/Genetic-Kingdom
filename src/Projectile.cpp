#include "Projectile.h"
#include <SDL2/SDL.h>
#include <cmath>
Projectile::Projectile(int x, int y, Enemy* target) : x(x), y(y), target(target) {}

void Projectile::update() {
    // moverse en linea recta hacia el objetivo
    double dx = target->getX() - x;
    double dy = target->getY() - y;
    double distance = sqrt(dx*dx + dy*dy);
    x += dx / distance * speed;
    y += dy / distance * speed;

    if (distance <= 10) {
        hit();
    }
}

void Projectile::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    SDL_Rect rect = { x - 2, y - 2, 4, 4 };
    SDL_RenderFillRect(renderer, &rect);
}

void Projectile::hit() {
    target->takeDamage(1);
    hasHit = true;
}

int Projectile::getX() const { return x; }
int Projectile::getY() const { return y; }
bool Projectile::getHasHit() const { return hasHit; }
