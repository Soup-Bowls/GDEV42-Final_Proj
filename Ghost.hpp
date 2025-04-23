#ifndef GHOST 
#define GHOST 

#include <raylib.h>
#include <raymath.h>
#include "Entity.hpp"
#include "EnemyBase.hpp"
#include "TileMap.hpp"

#define GAME_SCENE_SPRITE_GHOST "Assets/Sprites/ghost.png"

class Ghost;

class GhostState {
public: 
    virtual ~GhostState() {}
    virtual void Enter(Ghost& ghost) = 0;
    virtual void Update(Ghost& ghost, float delta_time) = 0;
    virtual void HandleCollision(Ghost& ghost, Entity* other_entity) = 0;
};

class GhostWandering: public GhostState {
public: 
    float change_direction_cooldown;
    Vector2 move_direction;
    void Enter (Ghost& ghost);
    void Update(Ghost& ghost, float delta_time);
    void HandleCollision(Ghost& ghost, Entity* other_entity);
};

class GhostChasing: public GhostState {
public:
    void Enter (Ghost& ghost);
    void Update(Ghost& ghost, float delta_time);
    void HandleCollision(Ghost& ghost, Entity* other_entity);   
};

class GhostAttacking: public GhostState{
public: 
    void Enter (Ghost& ghost);
    void Update(Ghost& ghost, float delta_time);
    void HandleCollision(Ghost& ghost, Entity* other_entity);   
};

class Ghost : public BaseEnemy {
public: 

    enum class Animation_type {
        WANDERING, 
        CHASING, 
        ATTACKING,
        DEATH
    };

    Animation_type animation_state;

    
    Rectangle ghostFrameRect;
    Rectangle ghostDR;

    int animationStartFrame;
    float hideTimer;
    bool playOnce; 


    GhostWandering wandering;
    GhostChasing chasing; 
    GhostAttacking attack;

    void setTileMap(TileMap* map) {
        tile_map = map;
    }

    Ghost(Vector2 pos, float spd, float rad, float d_radius, float a_radius, float r_radius, int hp);

    void Update(float delta_time) override;
    void Draw() override;
    void SetState(GhostState* new_state);
    void HandleCollision(Entity* other_entity) override;

private:
    GhostState* current_state;
    bool flash_visible;
    float flash_timer;
    float flash_interval;


};

#endif