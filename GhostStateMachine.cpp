#include <raylib.h>
#include <raymath.h>
#include <iostream>

#include "Ghost.hpp"

using namespace std;

void Ghost::Update(float delta_time) {
    int frameRange = maxFrames;
    animationTimer += delta_time;

    if(animationTimer >= frameSpeed && maxFrames > 1) {
        animationTimer = 0.0f;

        if (playOnce) {
            if (currentFrame < animationStartFrame + maxFrames - 1) {
                currentFrame++;
            }

        } else {
            currentFrame = animationStartFrame + ((currentFrame - animationStartFrame + 1) % frameRange);
        }
    }

    if (invulnerable_timer > 0.0f) {
        invulnerable_timer -= delta_time;

        flash_timer += delta_time;
        if (flash_timer >= flash_interval) {
            flash_visible = !flash_visible;
            flash_timer = 0.0f;
        }
    } else {
        flash_visible = true;
    }

    current_state->Update(*this, delta_time);

}

void Ghost::Draw() {
    if (!active || sprite.id == 0 || !flash_visible) return;

    Rectangle src = {
        frameWidth * currentFrame,
        frameHeight * direction,
        frameWidth,
        frameHeight,
    };

    Rectangle dst = {
        position.x,
        position.y,
        frameWidth,
        frameHeight
    };

    Vector2 origin = { frameWidth /2, frameHeight /2};

    DrawTexturePro(sprite, src, dst, origin, 0.0f, WHITE);
    DrawCircleLines(position.x, position.y, detection_radius, VIOLET);
    DrawCircleLines(position.x, position.y, aggro_radius, BLUE);
    DrawCircleLines(position.x, position.y, ready_attack_radius, RED);

    
    float bar_width = 40;
    float bar_height = 6;
    float bar_x = position.x - bar_width / 2;
    float bar_y = position.y - frameHeight / 2 - 10;

    float hp_percent = (float)health / maxHealth;
    DrawRectangle(bar_x, bar_y, bar_width, bar_height, DARKGRAY);
    DrawRectangle(bar_x, bar_y, bar_width * hp_percent, bar_height, MAROON);
    DrawRectangleLines(bar_x, bar_y, bar_width, bar_height, BLACK);
}

void Ghost::SetState(GhostState* new_state) {
    current_state = new_state;
    current_state->Enter(*this);
}

void Ghost::HandleCollision(Entity* other_entity) {
    current_state->HandleCollision(*this, other_entity);
}

Ghost::Ghost(Vector2 pos, float spd, float rad, float d_radius, float a_radius, float r_radius, int hp) {
    enemyID = 2;
    position = pos;
    speed = spd;
    radius = rad;
    detection_radius = d_radius;
    aggro_radius = a_radius;
    ready_attack_radius = r_radius;
    health = hp;
    maxHealth = hp;
    active = true;

    flash_visible = true;
    flash_timer = 0.0f;
    flash_interval = 0.1f;

    sprite = LoadTexture(GAME_SCENE_SPRITE_GHOST);
    frameWidth = sprite.width / 6;
    frameHeight = sprite.height / 4;
    currentFrame = 0;
    maxFrames = 6;
    animationStartFrame = 0;
    frameSpeed = .3f;
    direction = 0;

    hideTimer = 3.0f;

    SetState(&wandering);

}

void GhostWandering::Enter(Ghost& ghost) {
    ghost.playOnce = false;
    ghost.color = VIOLET;
    ghost.speed = 5.0f;
    change_direction_cooldown = GetRandomValue(1, 3);
    move_direction.x = GetRandomValue(-100.0f, 100.0f) / 100.0f;
    move_direction.y = GetRandomValue(-100.0f, 100.0f) / 100.0f;
    move_direction = Vector2Normalize(move_direction);
    ghost.entity_following = nullptr;

    if (ghost.hideTimer <= 0.0f) {
        ghost.animationStartFrame = 4;  
        ghost.maxFrames = 1;
    } else {
        ghost.animationStartFrame = 0;
        ghost.maxFrames = 3;
    }

    ghost.currentFrame = ghost.animationStartFrame;
    ghost.frameSpeed = 0.15f;
}


void GhostChasing::Enter(Ghost& ghost) {
    ghost.playOnce = false;
    ghost.color = YELLOW;
    ghost.animationStartFrame = 4;
    ghost.maxFrames = 2;
    ghost.currentFrame = ghost.animationStartFrame;
    ghost.frameSpeed = 0.2f;

}

void GhostAttacking::Enter(Ghost& ghost) {

}

void GhostWandering::Update(Ghost& ghost, float delta_time) {
    ghost.velocity = Vector2Zero();

    // Update hide timer
    if (ghost.hideTimer > 0.0f) {
        ghost.hideTimer -= delta_time;
    }


    if (ghost.hideTimer <= 0.0f && ghost.animationStartFrame != 4) {
        ghost.animationStartFrame = 3;
        ghost.maxFrames = 1;
        ghost.currentFrame = 3;
    } else if (ghost.hideTimer > 0.0f && ghost.animationStartFrame != 0) {
        ghost.animationStartFrame = 0;
        ghost.maxFrames = 3;
        ghost.currentFrame = 0;
    }

    if (change_direction_cooldown <= 0.0f) {
        move_direction.x = GetRandomValue(-100.0f, 100.0f) / 100.0f;
        move_direction.y = GetRandomValue(-100.0f, 100.0f) / 100.0f;
        move_direction = Vector2Normalize(move_direction);
        change_direction_cooldown = GetRandomValue(1, 3);
    } else {
        change_direction_cooldown -= delta_time;
    }

    ghost.velocity = Vector2Scale(move_direction, 50.0f);

    // Direction
    if (fabs(ghost.velocity.x) > fabs(ghost.velocity.y)) {
        ghost.direction = ghost.velocity.x < 0 ? 1 : 3;
    } else {
        ghost.direction = ghost.velocity.y < 0 ? 0 : 2;
    }

    Vector2 new_position = Vector2Add(ghost.position, Vector2Scale(ghost.velocity, delta_time));

    Ghost temp_ghost = ghost;
    temp_ghost.position = new_position;

    if (ghost.tile_map && !ghost.tile_map->CheckTileCollision(&temp_ghost)) {
        ghost.position = new_position;
    } else {
        move_direction = Vector2Normalize({GetRandomValue(-100, 100) / 100.0f, GetRandomValue(-100, 100) / 100.0f});
        change_direction_cooldown = GetRandomValue(1, 3);
    }

    if (ghost.invulnerable_timer > 0.0f) {
        ghost.invulnerable_timer -= delta_time;
    }

    if (ghost.health <= 0) {
        ghost.active = false;
    }
}

void GhostChasing::Update(Ghost& ghost, float delta_time) {
    ghost.velocity = Vector2Subtract(ghost.entity_following->position, ghost.position);
    ghost.velocity = Vector2Normalize(ghost.velocity);
    ghost.velocity = Vector2Scale(ghost.velocity, 100.0f);

    if (abs(ghost.velocity.x) > abs(ghost.velocity.y)){
        if (ghost.velocity.x < 0) {
            ghost.direction = 1;    // LEFT
        } 
        if (ghost.velocity.x > 0) {
            ghost.direction = 3;    //RIGHT
        }
    } else {
        if(ghost.velocity.y < 0) {
            ghost.direction = 0;    // UP
        }
        if(ghost.velocity.y > 0) {
            ghost.direction = 2;    // DOWN
        }
    }

    Vector2 new_position = Vector2Add(ghost.position, Vector2Scale(ghost.velocity, delta_time));

    Ghost temp_enemy = ghost;
    temp_enemy.position = new_position;

    if(ghost.tile_map && !ghost.tile_map->CheckTileCollision(&temp_enemy)) {
        ghost.position = new_position;
    } else {
        ghost.SetState(&ghost.wandering);
    }

    if (ghost.invulnerable_timer > 0.0f) {
        ghost.invulnerable_timer -= delta_time;
    }

    if (ghost.health <= 0) {
        ghost.active = false;
    }


}

void GhostAttacking::Update(Ghost& ghost, float delta_time) {
    if (ghost.invulnerable_timer > 0.0f) {
        ghost.invulnerable_timer -= delta_time;
    }

    if (ghost.health <= 0) {
        ghost.active = false;
    }
}

void GhostWandering::HandleCollision(Ghost& ghost, Entity* other_entity) {
    if (CheckCollisionCircles(ghost.position, ghost.detection_radius, other_entity->position, other_entity->radius)) {
        ghost.entity_following = other_entity;
        ghost.SetState(&ghost.chasing);
    }
}

void GhostChasing::HandleCollision(Ghost& ghost, Entity* other_entity) {
    if(!CheckCollisionCircles(ghost.position, ghost.aggro_radius, other_entity->position, other_entity->radius)) {
        ghost.SetState(&ghost.wandering);
    }

    if(CheckCollisionCircles(ghost.position, ghost.ready_attack_radius, other_entity->position, other_entity->radius)) {
        ghost.SetState(&ghost.attack);
    }
}

void GhostAttacking::HandleCollision(Ghost& ghost, Entity* other_entity) {
    if (!CheckCollisionCircles(ghost.position, ghost.ready_attack_radius, other_entity->position, other_entity->radius)) {
        ghost.SetState(&ghost.chasing);
    }
}
