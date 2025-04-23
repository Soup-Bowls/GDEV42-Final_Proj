#include <raylib.h>
#include <raymath.h>

#include "projectile.hpp"
#define GAME_SCENE_EYEBALL_PROJECTILE "Assets/Texture/orb.png"

void Projectile::Update(float delta_time) {
    position = Vector2Add(position, Vector2Scale(velocity, delta_time));
    if (position.x < 0 || position.x > GetScreenWidth() ||
        position.y < 0 || position.y > GetScreenHeight()) {
        active = false;
    }
};

void Projectile::Draw() {
    if (!active) return;

    Vector2 origin = { projectileSprite.width / 2.0f, projectileSprite.height / 2.0f };
    Rectangle src = { 0, 0, (float)projectileSprite.width, (float)projectileSprite.height };
    Rectangle dst = { position.x, position.y, (float)projectileSprite.width, (float)projectileSprite.height };

    DrawTexturePro(projectileSprite, src, dst, origin, 0.0f, WHITE);
}


Projectile::Projectile(Vector2 pos, Vector2 vel, float r, Texture2D orb, Color col) {
    position = pos;
    velocity = vel;
    radius = r;
    projectileSprite = orb;
    color = col;
    active = true;
}