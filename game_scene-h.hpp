#ifndef GAME_SCENE
#define GAME_SCENE

#include <raylib.h>
#include <raymath.h>
#include <string>
#include <iostream>
#include <vector>
#include "scene_manager.hpp"

class GameScene : public Scene {
public:
    GameScene();
    
    void Begin() override;
    void End() override;
    void Update() override;
    void Draw() override;

private:
    // Structures

    struct Screen {
        const float window_width = 800;
        const float window_height = 600;
    };

    struct Wall {
        Vector2 position;
        float width, height;
    };

    struct Bullet {
        Vector2 bulletPosition;
        Vector2 bulletDirection;
        float radius, shootCD, baseDamage, speed;
        bool isActive, canMove;
        int id;
        Color bulletColor;
    };

    struct Player {
        Vector2 position;
        float HP, radius, speed, collisionCD;
        bool isHit,  canShoot, canMove;
        Color playerColor;
        std::vector<Bullet> playerProjectile;
    };

    struct Enemy {
        Vector2 position;
        Rectangle frameRect, enemyDR;
        Texture2D sprite, deathTexture;
        float HP, damage, radius, speed, CD, attackTime, detectionRadius,frameHeight, frameWidth;
        bool isHit, isAlive, canAttack, isVulnerable, isAtPointA, isMoving;
        int id, cost, point;
        std::vector<Bullet> enemyProjectile;
        float hitTimer = 0.0f;
        Color enemyColor;
    };

    struct Skill {
        bool canCast, isCasting;
        float CD, duration, radius;
        Color color;
    };
    Vector2 direction = {0,0};

    Sound playerCollisionSound = {0};
    Music gameSceneMusic = {0};
    bool musicLoaded = false;
    bool soundLoaded = false;

    std::vector<Enemy> activeEnemies;
    void handleProjectileCollision(Bullet& b, Enemy& e);
    void handleEnemyProjectileCollision(Bullet& b, Player& p);
    void handleEnemyPlayerCollision(Player& p, Enemy& e);
    void spawnWave(float deltaTime);
    void updateWave(float deltaTime);

    Enemy createEnemy(int& type, Vector2& position);
    Bullet createEnemyProjectile(int& type, Vector2& position, Vector2& direction);
    Bullet createPlayerProjectile(Vector2& position, Vector2& direction);


    void checkProjectileCollision(Player& player, Enemy& enemies);
    void spawnEnemyProjectile(Enemy& e, Vector2& direction);
    void spawnPlayerProjectile(Player& p, Vector2& direction);
    void updatePlayerProjectile(Player& player, float deltaTime);
    void updateEnemyProjectile(Enemy& enemy, float delta_time);
    
    Rectangle destinationRect(float x, float y, float& width, float& height, float scale);
    Vector2 enemyDirection(Vector2& enemy, Vector2& player);

    Screen screen;
    Player player;
    Bullet playerBullet;
    Bullet beeProjectile;
    Skill skill;

    Texture2D playerTexture;
    Texture2D roomBackground;
    Texture2D bulletTexture;
    Texture2D slimeTexture;
    Texture2D heartTexture;
    Texture2D beeTexture;
    Texture2D ghostTexture;


    int playerFrame = 4;
    int slimeFrame = 8;
    float frameWidth, frameHeight;
    float heartFrameWidth, heartFrameHeight;
    float bulletFrameWidth, bulletFrameHeight;

    Rectangle playerFrameRec;
    Rectangle bulletFrameRect;
    Rectangle heartFrameRect;

    Rectangle bulletDR;
    Rectangle enemyDR;
    Rectangle heartDR;
    Rectangle playerDR;

    float bulletSpeed = 450.0f;
    float shootCounter = 0.0f;
    float skillCounter = 0.0f;
    float castingCounter = 0.0f;
    float slimeDeathCounter = 0;

    int currentFrame = 0;
    float delta_time;
    float frameDuration = 0.1f;
    float animationTime = 0.0f;
    
    int currentWave = 1;
    int pointRemaining = 10;
    bool waveActive  = true;
    float waveCD = 3.00f;
    float waveTimer = 0.0f;
    float spawnTimer;
    float spawnInterval;
    int gamePoint = 0;
};

#endif