#include "Projectile.h"
#include <SDL2/SDL.h>
#include <cmath>

// Clase base
Projectile::Projectile(int x, int y, Enemy* target, int damage)
    : x(x), y(y), target(target), damage(damage) {}

void Projectile::update() {
    if (!target || target->isDead()) {
        hit = true;
        return;
    }

    float dx = target->getX() - x;
    float dy = target->getY() - y;
    float dist = std::sqrt(dx * dx + dy * dy);

    if (dist < 5.0f) {
        // Aplica daño según el tipo de proyectil
        if (dynamic_cast<Arrow*>(this)) {
            target->takeDamage(damage, "arrow");
        } else if (dynamic_cast<Magic*>(this)) {
            target->takeDamage(damage, "magic");
        } else if (dynamic_cast<Bullet*>(this)) {
            target->takeDamage(damage, "artillery");
        }
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
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Color por defecto 
    SDL_Rect rect = { static_cast<int>(x) - 2, static_cast<int>(y) - 2, 4, 4 };
    SDL_RenderFillRect(renderer, &rect);
}

// Flecha (proyectil blanco)
Arrow::Arrow(int x, int y, Enemy* target, int damage)
    : Projectile(x, y, target, damage) {}

void Arrow::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Blanco
    SDL_Rect rect = { static_cast<int>(x) - 2, static_cast<int>(y) - 2, 4, 4 };
    SDL_RenderFillRect(renderer, &rect);
}

// Magia (proyectil naranja)
Magic::Magic(int x, int y, Enemy* target, int damage)
    : Projectile(x, y, target, damage) {}

void Magic::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255); // Naranja
    SDL_Rect rect = { static_cast<int>(x) - 2, static_cast<int>(y) - 2, 4, 4 };
    SDL_RenderFillRect(renderer, &rect);
}

// Bala (proyectil gris)
Bullet::Bullet(int x, int y, Enemy* target, int damage)
    : Projectile(x, y, target, damage) {}

void Bullet::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255); // Gris
    SDL_Rect rect = { static_cast<int>(x) - 2, static_cast<int>(y) - 2, 4, 4 };
    SDL_RenderFillRect(renderer, &rect);
}

// Proyectil especial (proyectil rojo)
SpecialProjectile::SpecialProjectile(int x, int y, Enemy* target, int damage)
    : Projectile(x, y, target, damage) {}

void SpecialProjectile::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Rojo
    SDL_Rect rect = { static_cast<int>(x) - 8, static_cast<int>(y) - 8, 16, 16 }; // Tamaño aumentado
    SDL_RenderFillRect(renderer, &rect);
}


