#include "Projectile.h"
#include <SDL2/SDL.h>
#include <cmath>

// Clase base
Projectile::Projectile(int x, int y, Enemy* target, int damage, const std::string& type)
    : x(x), y(y), target(target), damage(damage), type(type) {}

void Projectile::update() {
    if (!target || target->isDead()) {
        hit = true; // Marcar como impactado si el objetivo ya no existe o está muerto
        return;
    }

    // Movimiento hacia el objetivo
    float dx = target->getX() - x;
    float dy = target->getY() - y;
    float dist = std::sqrt(dx * dx + dy * dy);

    if (dist <= speed) {
        // Si el proyectil alcanza el objetivo
        target->takeDamage(damage, type);
        hit = true; // Marcar como impactado
    } else {
        // Movimiento hacia el objetivo
        x += (dx / dist) * speed;
        y += (dy / dist) * speed;
    }
}

bool Projectile::hasHit() const {
    return hit;
}

void Projectile::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Color por defecto 
    SDL_Rect rect = { static_cast<int>(x) - 2, static_cast<int>(y) - 2, 4, 4 };
    SDL_RenderFillRect(renderer, &rect);
}

// Flecha (proyectil blanco)
Arrow::Arrow(int x, int y, Enemy* target, int damage)
    : Projectile(x, y, target, damage, "arrow") {}

void Arrow::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Blanco
    SDL_Rect rect = { static_cast<int>(x) - 2, static_cast<int>(y) - 2, 4, 4 };
    SDL_RenderFillRect(renderer, &rect);
}

// Magia (proyectil naranja)
Magic::Magic(int x, int y, Enemy* target, int damage)
    : Projectile(x, y, target, damage, "magic") {}

void Magic::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255); // Naranja
    SDL_Rect rect = { static_cast<int>(x) - 2, static_cast<int>(y) - 2, 4, 4 };
    SDL_RenderFillRect(renderer, &rect);
}

// Bala (proyectil gris)
Bullet::Bullet(int x, int y, Enemy* target, int damage)
    : Projectile(x, y, target, damage, "artillery") {}

void Bullet::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255); // Gris
    SDL_Rect rect = { static_cast<int>(x) - 2, static_cast<int>(y) - 2, 4, 4 };
    SDL_RenderFillRect(renderer, &rect);
}

// Proyectil especial (proyectil rojo)
SpecialProjectile::SpecialProjectile(int x, int y, Enemy* target, int damage)
    : Projectile(x, y, target, damage, "special") {}

void SpecialProjectile::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Rojo
    SDL_Rect rect = { static_cast<int>(x) - 4, static_cast<int>(y) - 4, 8, 8 }; // Más grande
    SDL_RenderFillRect(renderer, &rect);
}


