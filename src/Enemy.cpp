#include "Enemy.h"
#include <SDL2/SDL.h>

Enemy::Enemy(int x, int y) : x(x), y(y) {}

void Enemy::update() {
    x += speed;
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
