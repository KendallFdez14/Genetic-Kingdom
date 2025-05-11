#pragma once
#include <SDL2/SDL.h>
#include "Enemy.h"

class Projectile {
    public:
        Projectile(int x, int y, Enemy* target, int damage);
        virtual ~Projectile() = default;

        virtual void render(SDL_Renderer* renderer); // Método virtual para personalizar el renderizado
        void update();
        bool hasHit() const;

    protected:
        float x, y;
        float speed = 4.0f;
        int damage;
        Enemy* target;
        bool hit = false;
};

// Flecha (proyectil blanco)
class Arrow : public Projectile {
    public:
        Arrow(int x, int y, Enemy* target, int damage);
        void render(SDL_Renderer* renderer) override;
};

// Magia (proyectil naranja)
class Magic : public Projectile {
    public:
        Magic(int x, int y, Enemy* target, int damage);
        void render(SDL_Renderer* renderer) override;
};

// Bala (proyectil gris)
class Bullet : public Projectile {
    public:
        Bullet(int x, int y, Enemy* target, int damage);
        void render(SDL_Renderer* renderer) override;
};

// Proyectil especial
class SpecialProjectile : public Projectile {
    public:
        SpecialProjectile(int x, int y, Enemy* target, int damage);
        void render(SDL_Renderer* renderer) override;
};
