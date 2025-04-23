#include <raylib.h>
#include <raymath.h>
#include <iostream>

#include "Slime.hpp"

using namespace std;

void Slime::Update(float delta_time) {
    int frameRange = maxFrames;
    animationTimer += delta_time;

    if (current_state == nullptr) {
        std::cerr << "ERROR: Slime current_state is NULL!\n";
        return;
    }
    // else {
    //     cout << "STATE: No error in Slime Update Function" << endl;
    // }

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

void Slime::Draw() {
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

void Slime::SetState(SlimeState* new_state) {
    current_state = new_state;
    current_state->Enter(*this);
}

void Slime::HandleCollision(Entity* other_entity) {
    current_state->HandleCollision(*this, other_entity);
}

Slime::Slime(Vector2 pos, float spd, float rad, float d_radius, float a_radius, float r_radius, int hp) {
    enemyID = 0;
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

    sprite = LoadTexture(GAME_SCENE_SPRITE_SLIME);
    frameWidth = (float) (sprite.width/ 35);
    frameHeight = (float) (sprite.height /4);
    currentFrame = 0;
    maxFrames = 35;
    animationStartFrame = 0;
    frameSpeed = .3f;
    direction = 0;

    SetState(&wandering);
}

void slimeWandering::Enter(Slime& slime) {
    cout << "slime wandering" << endl;
    slime.playOnce = false;
    slime.color = VIOLET;
    slime.speed = 5.0f;
    change_direction_cooldown = GetRandomValue(1, 3);
    move_direction.x = GetRandomValue(-100.0f, 100.0f) / 100.0f;
    move_direction.y = GetRandomValue(-100.0f, 100.0f) / 100.0f;

    move_direction = Vector2Normalize(move_direction);
    slime.entity_following = nullptr;

    slime.animationStartFrame = 27;
    slime.maxFrames = 8;
    slime.currentFrame = slime.animationStartFrame;
    slime.frameSpeed = 0.15f;
}

void slimeChasing::Enter(Slime& slime) {
    cout << "slime wandering" << endl;
    slime.playOnce = false;
    slime.color = YELLOW;
    slime.speed = 10.0f;

    slime.animationStartFrame = 19;
    slime.maxFrames = 8;
    slime.currentFrame = slime.animationStartFrame;
    slime.frameSpeed = 0.15f;

    
}

void slimeAttacking::Enter(Slime&slime) {
    cout << "slime attacking" << endl;
    slime.playOnce = true;
    slime.color = RED;

    slime.animationStartFrame = 0;
    slime.maxFrames = 10;
    slime.currentFrame = slime.animationStartFrame;
    slime.frameSpeed = 0.1f;
}

void slimeWandering::Update(Slime& slime, float delta_time) {
    slime.velocity = Vector2Zero();

    if (change_direction_cooldown <= 0.0f) {
        move_direction.x = GetRandomValue(-100.0f, 100.0f) / 100.0f;
        move_direction.y = GetRandomValue(-100.0f, 100.0f) / 100.0f;

        move_direction = Vector2Normalize(move_direction);
        
        change_direction_cooldown = GetRandomValue(1, 3);
    }
    else {
        change_direction_cooldown -= delta_time;
    }
    slime.velocity = Vector2Scale(move_direction, 50.0f);

    if (abs(slime.velocity.x) > abs(slime.velocity.y)){
        if (slime.velocity.x < 0) {
            slime.direction = 2;    // LEFT
        } 
        if (slime.velocity.x > 0) {
            slime.direction = 3;    //RIGHT
        }
    } else {
        if(slime.velocity.y < 0) {
            slime.direction = 1;    // UP
        }
        if(slime.velocity.y > 0) {
            slime.direction = 0;    // DOWN
        }
    }

    Vector2 new_position = Vector2Add(slime.position, Vector2Scale(slime.velocity, delta_time));

    Slime temp_enemy = slime;
    temp_enemy.position = new_position;

    if (slime.tile_map && !slime.tile_map->CheckTileCollision(&temp_enemy)) {
        slime.position = new_position;
    } else {
        // Pick a new random direction if collision happens
        move_direction.x = GetRandomValue(-100, 100) / 100.0f;
        move_direction.y = GetRandomValue(-100, 100) / 100.0f;
        move_direction = Vector2Normalize(move_direction);
        change_direction_cooldown = GetRandomValue(1, 3);  // reset cooldown
    }
    

    if (slime.invulnerable_timer > 0.0f) {
        slime.invulnerable_timer -= delta_time;
    }

    if (slime.health <= 0) {
        slime.active = false;
    }
}

void slimeChasing::Update(Slime& slime, float delta_time) {
    slime.velocity = Vector2Subtract(slime.entity_following->position, slime.position);
    slime.velocity = Vector2Normalize(slime.velocity);
    slime.velocity = Vector2Scale(slime.velocity, 100);

    if (abs(slime.velocity.x) > abs(slime.velocity.y)){
        if (slime.velocity.x < 0) {
            slime.direction = 2;    // LEFT
        } 
        if (slime.velocity.x > 0) {
            slime.direction = 3;    //RIGHT
        }
    } else {
        if(slime.velocity.y < 0) {
            slime.direction = 1;    // UP
        }
        if(slime.velocity.y > 0) {
            slime.direction = 0;    // DOWN
        }
    }

    Vector2 new_position = Vector2Add(slime.position, Vector2Scale(slime.velocity, delta_time));

    Slime temp_enemy = slime;
    temp_enemy.position = new_position;

    if(slime.tile_map && !slime.tile_map->CheckTileCollision(&temp_enemy)) {
        slime.position = new_position;
    } else {
        slime.SetState(&slime.wandering);
        }

    if (slime.invulnerable_timer > 0.0f) {
        slime.invulnerable_timer -= delta_time;
    }

    if (slime.health <= 0) {
        slime.active = false;
    }


}

void slimeAttacking::Update(Slime&slime, float delta_time) {
    if (slime.invulnerable_timer > 0.0f) {
        slime.invulnerable_timer -= delta_time;
    }

    if (slime.health <= 0) {
        slime.active = false;
    }

}

void slimeWandering::HandleCollision(Slime& slime, Entity* other_entity) {
    if (CheckCollisionCircles(slime.position, slime.detection_radius, other_entity->position, other_entity->radius)) {
        slime.entity_following = other_entity;
        slime.SetState(&slime.chasing);
    }
}

void slimeChasing::HandleCollision(Slime& slime, Entity* other_entity) {
    if(!CheckCollisionCircles(slime.position, slime.aggro_radius, other_entity->position, other_entity->radius)) {
        slime.SetState(&slime.wandering);
    }

    if(CheckCollisionCircles(slime.position, slime.ready_attack_radius, other_entity->position, other_entity->radius)) {
        slime.SetState(&slime.attack);
    }
}

void slimeAttacking::HandleCollision(Slime&slime, Entity* other_entity) {
    if(!CheckCollisionCircles(slime.position, slime.ready_attack_radius, other_entity->position, other_entity->radius)) {
        if (slime.currentFrame >= slime.animationStartFrame + slime.maxFrames - 1) {
            slime.SetState(&slime.chasing);
        }
    }
}