#include <raylib.h>
#include <raymath.h>
#include <iostream>

#include "Bee.hpp"


void Bee::Update(float delta_time) {
    if (!active || current_state == nullptr) return;

    animationTimer += delta_time;

    if(animationTimer >= frameSpeed && maxFrames > 1) {
        animationTimer = 0.0f;

        if(animation_state == Animation_type::WANDERING) {
            currentFrame = (currentFrame + 1) % maxFrames;
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

void Bee::Draw() {
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

    Vector2 origin = { frameWidth / 2, frameHeight / 2 };

    DrawTexturePro(sprite, src, dst, origin, 0.0f, WHITE);

    DrawCircleLines(position.x, position.y, ready_attack_radius, DARKBLUE);
    DrawCircleLines(position.x, position.y, detection_radius, VIOLET);
    DrawCircleLines(position.x, position.y, aggro_radius, BLUE);

    float bar_width = 40;
    float bar_height = 6;
    float bar_x = position.x - bar_width / 2;
    float bar_y = position.y - frameHeight / 2 - 10;

    float hp_percent = (float)health / maxHealth;
    DrawRectangle(bar_x, bar_y, bar_width, bar_height, DARKGRAY);
    DrawRectangle(bar_x, bar_y, bar_width * hp_percent, bar_height, MAROON);
    DrawRectangleLines(bar_x, bar_y, bar_width, bar_height, BLACK);
}


void Bee::SetState(BeeState* new_state) {
    current_state = new_state;
    current_state->Enter(*this);
}

void Bee::HandleCollision(Entity* other_entity) {
    current_state->HandleCollision(*this, other_entity);
}

Bee::Bee(Vector2 pos, float spd, float rad, float d_radius, float a_radius, float r_radius, int hp) {
    position = pos;
    speed = spd;
    radius = rad;
    health = hp;
    maxHealth = hp;
    active = true;

    detection_radius = d_radius;
    aggro_radius = a_radius;
    ready_attack_radius = r_radius;

    flash_visible = true;
    flash_timer = 0.0f;
    flash_interval = 0.1f;
    

    sprite = LoadTexture(GAME_SCENE_SPRITE_BEE);
    frameWidth = sprite.width / 6.0f;
    frameHeight = sprite.height / 4.0f;
    maxFrames = 6;
    currentFrame = 0;
    frameSpeed = 0.3f;

    SetState(&wandering);
}


void BeeWandering::Enter(Bee& bee) {
    std::cout << "Entering wandering state for Bee. Texture ID: " << bee.sprite.id << std::endl;

    bee.color = VIOLET;
    change_direction_cooldown = GetRandomValue(1, 3);
    move_direction.x = GetRandomValue(-100.0f, 100.0f) / 100.0f;
    move_direction.y = GetRandomValue(-100.0f, 100.0f) / 100.0f;

    move_direction = Vector2Normalize(move_direction);
    bee.entity_following = nullptr;

    bee.currentFrame = 0;
    bee.maxFrames = 3;
}

void BeeChasing::Enter(Bee& bee) {
    bee.color = YELLOW;
}

void BeeReady::Enter(Bee& bee) {
    bee.color = ORANGE;
    ready_timer = 1.0f;
    bee.currentFrame = 3;
    bee.maxFrames = 1;
}

void BeeAttacking::Enter(Bee& bee) {
    bee.color = RED;

    attack_direction = Vector2Scale(Vector2Normalize(Vector2Subtract(bee.entity_following->position, bee.position)), 1000.0f);
    bee.acceleration = attack_direction;

    bee.currentFrame = 4;
    bee.maxFrames = 1;
}


void BeeWandering::Update(Bee& bee, float delta_time) {
    bee.velocity = Vector2Zero();

    if (change_direction_cooldown <= 0.0f) {
        move_direction.x = GetRandomValue(-100.0f, 100.0f) / 100.0f;
        move_direction.y = GetRandomValue(-100.0f, 100.0f) / 100.0f;

        move_direction = Vector2Normalize(move_direction);
        
        change_direction_cooldown = GetRandomValue(1, 3);
    }
    else {
        change_direction_cooldown -= delta_time;
    }

    bee.velocity = Vector2Scale(move_direction, 50.0f);

    if (abs(bee.velocity.x) > abs(bee.velocity.y)){
        if (bee.velocity.x < 0) {
            bee.direction = 1;    // LEFT
        } 
        if (bee.velocity.x > 0) {
            bee.direction = 3;    //RIGHT
        }
    } else {
        if(bee.velocity.y < 0) {
            bee.direction = 0;    // UP
        }
        if(bee.velocity.y > 0) {
            bee.direction = 2;    // DOWN
        }
    }

    Vector2 new_position = Vector2Add(bee.position, Vector2Scale(bee.velocity, delta_time));

    Bee temp_enemy = bee;
    temp_enemy.position = new_position;

    if (bee.tile_map && !bee.tile_map->CheckTileCollision(&temp_enemy)) {
        bee.position = new_position;
    } else {
        // Pick a new random direction if collision happens
        move_direction.x = GetRandomValue(-100, 100) / 100.0f;
        move_direction.y = GetRandomValue(-100, 100) / 100.0f;
        move_direction = Vector2Normalize(move_direction);
        change_direction_cooldown = GetRandomValue(1, 3);  // reset cooldown
    }
    

    if (bee.invulnerable_timer > 0.0f) {
        bee.invulnerable_timer -= delta_time;
    }

    if (bee.health <= 0) {
        bee.active = false;
    }
}

void BeeWandering::HandleCollision(Bee& bee, Entity* other_entity) {
    if (CheckCollisionCircles(bee.position, bee.detection_radius, other_entity->position, other_entity->radius)) {
        bee.entity_following = other_entity;
        bee.SetState(&bee.chasing);
    }
}

void BeeChasing::Update(Bee& bee, float delta_time) {

    bee.velocity = Vector2Subtract(bee.entity_following->position, bee.position);
    bee.velocity = Vector2Normalize(bee.velocity);
    bee.velocity = Vector2Scale(bee.velocity, 100.0f);

    if (abs(bee.velocity.x) > abs(bee.velocity.y)){
        if (bee.velocity.x < 0) {
            bee.direction = 1;    // LEFT
        } 
        if (bee.velocity.x > 0) {
            bee.direction = 3;    //RIGHT
        }
    } else {
        if(bee.velocity.y < 0) {
            bee.direction = 0;    // UP
        }
        if(bee.velocity.y > 0) {
            bee.direction = 2;    // DOWN
        }
    }
    
    Vector2 new_position = Vector2Add(bee.position, Vector2Scale(bee.velocity, delta_time));
    Bee temp_enemy = bee;
    temp_enemy.position = new_position;
    
    if (bee.tile_map && !bee.tile_map->CheckTileCollision(&temp_enemy)) {
        bee.position = new_position;
    } else {
        bee.SetState(&bee.wandering);
    }
    
    if (bee.invulnerable_timer > 0.0f) {
        bee.invulnerable_timer -= delta_time;
    }

    if (bee.health <= 0) {
        bee.active = false;
    }
}

void BeeChasing::HandleCollision(Bee& bee, Entity* other_entity) {
    if (CheckCollisionCircles(bee.position, bee.ready_attack_radius, other_entity->position, other_entity->radius)) {
        bee.SetState(&bee.ready);
    }
    
    if (!CheckCollisionCircles(bee.position, bee.aggro_radius, other_entity->position, other_entity->radius)) {
        bee.SetState(&bee.wandering);
    }
}

void BeeReady::Update(Bee& bee, float delta_time) {
    aim_direction = Vector2Subtract(bee.entity_following->position, bee.position);
    aim_direction = Vector2Normalize(aim_direction);


    if (!(ready_timer <= 0.0f)) {
        ready_timer -= delta_time;
    }
    else {
        bee.SetState(&bee.attacking);
    }

    if (bee.invulnerable_timer > 0.0f) {
        bee.invulnerable_timer -= delta_time;
    }

    if (bee.health <= 0) {
        bee.active = false;
    }
}

void BeeReady::HandleCollision(Bee& bee, Entity* other_entity) {
}

void BeeAttacking::Update(Bee& bee, float delta_time) {
    bee.velocity = Vector2Add(bee.velocity, bee.acceleration);
    bee.velocity = Vector2Subtract(bee.velocity, Vector2Scale(bee.velocity, 5.0f * delta_time));

    if (abs(bee.velocity.x) > abs(bee.velocity.y)){
        if (bee.velocity.x < 0) {
            bee.direction = 1;    // LEFT
        } 
        if (bee.velocity.x > 0) {
            bee.direction = 3;    //RIGHT
        }
    } else {
        if(bee.velocity.y < 0) {
            bee.direction = 0;    // UP
        }
        if(bee.velocity.y > 0) {
            bee.direction = 2;    // DOWN
        }
    }

    Vector2 new_position = Vector2Add(bee.position, Vector2Scale(bee.velocity, delta_time));
    Bee temp_enemy = bee;
    temp_enemy.position = new_position;

    if (bee.tile_map && !bee.tile_map->CheckTileCollision(&temp_enemy)) {
        bee.position = new_position;

        if (Vector2Length(bee.velocity) < 50.0f) {
            bee.velocity = Vector2Zero();
            bee.SetState(&bee.wandering);
        }
    } else {
        bee.velocity = Vector2Zero();
        bee.SetState(&bee.wandering);
    }
    

    bee.acceleration = Vector2Zero();

    if (bee.invulnerable_timer > 0.0f) {
        bee.invulnerable_timer -= delta_time;
    }

    if (bee.health <= 0) {
        bee.active = false;
    }
}

void BeeAttacking::HandleCollision(Bee& bee, Entity* other_entity) {
}