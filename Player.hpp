#ifndef PLAYER
#define PLAYER

#define GAME_SCENE_SPRITE_EYEBALL "Assets/Sprites/eyeball.png"
#define GAME_SCENE_EYEBALL_PROJECTILE "Assets/Texture/orb.png"
#define GAME_SCENE_HEART "Assets/Texture/heartscreen.png"
#define GAME_SCENE_COLLISION_SFX "Assets/Audio/Sounds/collision.wav"
#define GAME_SCENE_DAMAGE_SFX "Assets/Audio/Sounds/playerDamage.ogg"
#define GAME_SCENE_DODGE_SFX "Assets/Audio/Sounds/dodgeSound.wav"


#include <raylib.h>
#include <raymath.h>
#include <vector>
#include "Entity.hpp"
#include "TileMap.hpp"
#include "projectile.hpp"

class Player;

// Base Player State
// All Player States inherit this interface
class PlayerState {
public:
    virtual ~PlayerState() {}
    virtual void Enter(Player& player) = 0;
    virtual void Update(Player& player, float delta_time) = 0;
    virtual void HandleCollision(Player& player, Entity* other_entity) = 0;
};

class PlayerIdle : public PlayerState {
public:
    void Enter(Player& player);
    void Update(Player& player, float delta_time);
    void HandleCollision(Player& player, Entity* other_entity);
};

class PlayerMoving : public PlayerState {
public:
    void Enter(Player& player);
    void Update(Player& player, float delta_time);
    void HandleCollision(Player& player, Entity* other_entity);
};

class PlayerBlocking : public PlayerState {
public:
    void Enter(Player& player);
    void Update(Player& player, float delta_time);
    void HandleCollision(Player& player, Entity* other_entity);
};

class PlayerAttacking : public PlayerState {
public:
    float active_time;
    void Enter(Player& player);
    void Update(Player& player, float delta_time);
    void HandleCollision(Player& player, Entity* other_entity);
};

class PlayerDodging : public PlayerState {
public:
    Vector2 dodge_direction;
    void Enter(Player& player);
    void Update(Player& player, float delta_time);
    void HandleCollision(Player& player, Entity* other_entity);
};


// Base Player Class
class Player : public Entity {
public:
    std::vector<Projectile> projectiles;

    enum class Animation_type {
        IDLE, 
        MOVING
    };

    Animation_type animation_state;

    TileMap*  tile_map;

    Texture2D playerSprite;
    Texture2D projectileSprite;
    Texture2D heartSprite;

    Sound projectileSFX;
    Sound damageSFX;
    Sound dodgeSFX;


    Rectangle playerFrameRect;
    Rectangle playerDR;
    int currentFrame;
    float frameWidth, frameHeight;
    int direction;                
    float animationTimer;         
    float frameSpeed;            
    int maxFrames;                

    Vector2 velocity;
    Vector2 acceleration;
    float speed;
    bool in_attacking;
    float attack_radius;

    void setTileMap(TileMap* map) {
        tile_map = map;
    }
    Player(Vector2 pos, float rad, float spd, int hp);

    void Update(float delta_time);

    void Draw();

    void SetState(PlayerState* new_state);

    void HandleCollision(Entity* other_entity);

    PlayerIdle idle;
    PlayerMoving moving;
    PlayerBlocking blocking;
    PlayerAttacking attacking;
    PlayerDodging dodging;

private:
    PlayerState* current_state;
};


#endif