#pragma once
#include <SDL2/SDL.h>
#include "Enemy.h"
#include <string>

class Projectile {
public:
    Projectile(int x, int y, Enemy* target, int damage, const std::string& type);
    virtual void update();
    virtual void render(SDL_Renderer* renderer) = 0; // Método virtual puro
    bool hasHit() const;

protected:
    int x, y;
    int damage;
    float speed = 5.0f; // Velocidad del proyectil
    bool hit = false;
    Enemy* target;
    std::string type; // Tipo de daño (arrow, magic, artillery)
};

// Clases derivadas
class Arrow : public Projectile {
public:
    Arrow(int x, int y, Enemy* target, int damage);
    void render(SDL_Renderer* renderer) override;
};

class Magic : public Projectile {
public:
    Magic(int x, int y, Enemy* target, int damage);
    void render(SDL_Renderer* renderer) override;
};

class Bullet : public Projectile {
public:
    Bullet(int x, int y, Enemy* target, int damage);
    void render(SDL_Renderer* renderer) override;
};

class SpecialProjectile : public Projectile {
public:
    SpecialProjectile(int x, int y, Enemy* target, int damage);
    void render(SDL_Renderer* renderer) override;
};
