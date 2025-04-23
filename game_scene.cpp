#include "game_scene-h.hpp"
#include "scene_manager.hpp"
#include "highscore.hpp"
#include "name_entry_scene-h.hpp"
#include <iostream>
#include <fstream>
#include <vector>

GameScene::GameScene() {
}

void GameScene::Begin() {
    //more debug
    std::cout << "GameScene::Begin() - START" << std::endl;

    //load textures
    TextureData playerTextureData = ResourceManager::GetInstance()->GetTexture("eyeball.png");
    playerTexture = playerTextureData.texture;

    TextureData roomBackgroundTextureData = ResourceManager::GetInstance()->GetTexture("background.png");
    roomBackground = roomBackgroundTextureData.texture;

    TextureData bulletTextureData = ResourceManager::GetInstance()->GetTexture("orb.png");
    bulletTexture = bulletTextureData.texture;

    TextureData slimeTextureData = ResourceManager::GetInstance()->GetTexture("slime.png");
    slimeTexture = slimeTextureData.texture;

    TextureData heartTextureData = ResourceManager::GetInstance()->GetTexture("heartscreen.png");
    heartTexture = heartTextureData.texture;

    TextureData beeTextureData = ResourceManager::GetInstance()->GetTexture("bee.png");
    beeTexture = beeTextureData.texture;

    TextureData ghostTextureData = ResourceManager::GetInstance()->GetTexture("ghost.png");
    ghostTexture = ghostTextureData.texture;

    if(!IsMusicReady(gameSceneMusic)) {
        gameSceneMusic = LoadMusicStream("symphony.ogg");
    }

    if(IsMusicReady(gameSceneMusic)) {
        musicLoaded = true;
        AudioManager::GetInstance() -> SetCurrentMusic(gameSceneMusic);
        PlayMusicStream(gameSceneMusic);
    } else {
        std::cerr << "Failed to load game scene theme music" << std::endl;
    }
    

    if(!IsSoundReady(playerCollisionSound)) {
        playerCollisionSound = LoadSound("collision.wav");
    }

    if (IsSoundReady(playerCollisionSound)) {
        soundLoaded = false;
        AudioManager::GetInstance() -> SetCurrentSound(playerCollisionSound);
    } else {
        std::cerr << "Failed to load game scene theme music" << std::endl;
    }

    
    
    frameWidth = playerTexture.width / playerFrame;
    frameHeight = playerTexture.height/ playerFrame;

    heartFrameWidth = heartTexture.width / 7;
    heartFrameHeight = heartTexture.height;

    bulletFrameWidth = bulletTexture.width;
    bulletFrameHeight = bulletTexture.height;

    //player
        player.position = {screen.window_width/2, screen.window_height /2};
        player.radius = 35.0f;
        player.playerColor = RED;
        player.speed = 200.0f;
        player.HP = 70.0f;
        player.isHit = false;
        player.collisionCD = 0.0f;
        player.canShoot = true;

    //animation frames

        playerFrameRec = { 0.0f, 0.0f, frameWidth, frameHeight };
        bulletFrameRect = {0.0f, 0.0f, bulletFrameWidth, bulletFrameHeight};
        heartFrameRect = {0.0f, 0.0f, heartFrameWidth, heartFrameHeight};

    //debug help
    
    std::cout << "GameScene::Begin() - EXITING" << std::endl;
}

void GameScene::End() {
    currentWave = 0;
    activeEnemies.clear();

    std::ofstream file("result.txt");
    file << gamePoint << std::endl;

    gamePoint = 0;
    

    std::cout << "GameScene end called" << std::endl;

    if (IsMusicReady(gameSceneMusic)) {
        StopMusicStream(gameSceneMusic);
        UnloadMusicStream(gameSceneMusic);
        
        gameSceneMusic = {0};
        musicLoaded = false;
    }
    if (IsSoundReady(playerCollisionSound)) {
        StopSound(playerCollisionSound);
        UnloadSound(playerCollisionSound);
        
        playerCollisionSound = {0};
        soundLoaded = false;
    }
}

void GameScene::Update() {

    //debug help
    std::cout << "GameScene::Update() - CALLED" << std::endl;
    
    if(player.HP == 0) {
    if(highScoreManager.IsHighScore(gamePoint)) {
        if(GetSceneManager() != nullptr) {
            NameEntryScene* nameEntryScene = 
                dynamic_cast<NameEntryScene*>(GetSceneManager()->GetSceneByID(7)); // Assuming ID 7 for name entry scene
            
            if(nameEntryScene) {
                nameEntryScene->SetPlayerScore(gamePoint);
                GetSceneManager()->SwitchScene(7); // Switch to name entry scene
            } else {
                GetSceneManager()->SwitchScene(4);
            }
        }
    } else {
        if(GetSceneManager() != nullptr) {
            GetSceneManager()->SwitchScene(4);
        }
    }
}

    if (musicLoaded && IsMusicReady(gameSceneMusic)) {
        UpdateMusicStream(gameSceneMusic);

        if (!IsMusicStreamPlaying(gameSceneMusic)) {
            PlayMusicStream(gameSceneMusic);
        }
    }

    delta_time = GetFrameTime();

    animationTime += delta_time;

    if (animationTime >= frameDuration) {
        animationTime = 0.0f;
        currentFrame++;
        if (currentFrame >= 7 ) {
            currentFrame = 0; 
        }
    }


    updateWave(delta_time);



    // Update each active enemy (example)
    for (auto &enemy : activeEnemies) {
        if (enemy.isAlive) {
            Vector2 targetPoint;
            Vector2 pointA = {100, 100};
            Vector2 pointB = {600, 400};
            Vector2 bulletDirection = Vector2Normalize(enemyDirection(player.position,enemy.position));
            handleEnemyPlayerCollision(player, enemy);
            checkProjectileCollision(player, enemy);

            // Movement and target point assignment
            switch (enemy.id) {
                case 0:  // Slime
                    targetPoint = player.position;  // Slime always moves towards the player
                    enemy.isMoving = true;         // Ensure the slime is allowed to move
                    enemy.isVulnerable = false;
                    break;

                case 1:  // Bee
                    targetPoint = enemy.isAtPointA ? pointB : pointA;  // Bee moves between two points
                    enemy.isVulnerable = false;
                    break;

                case 2:  // Ghost
                    targetPoint = player.position;  // Ghost moves towards the player
                    enemy.isMoving = true;         // Ensure the ghost is allowed to move
                    break;

                default:
                    continue;  
            }

            if(enemy.HP <= 0) {
                enemy.position.x = enemy.position.x;
                enemy.position.y = enemy.position.y;
                enemy.isAlive = false;
                gamePoint += enemy.point;
            }

            // Move if the enemy is marked as moving
            if (enemy.isMoving) {
                Vector2 direction = enemyDirection(targetPoint, enemy.position);
                direction = Vector2Normalize(direction);

                // Update enemy position
                enemy.position.x += direction.x * enemy.speed * delta_time;
                enemy.position.y += direction.y * enemy.speed * delta_time;

                if (Vector2Distance(enemy.position, targetPoint) < 10.0f) {
                    enemy.isMoving = false;  

                    if (enemy.id == 1 && enemy.CD <= 0.0f) {  
                        spawnEnemyProjectile(enemy,bulletDirection);
                        enemy.CD = 5.0f;  
                    }
                }
            }


            // Update projectiles' positions
            for (auto &bullet : enemy.enemyProjectile) {  
                handleEnemyProjectileCollision(bullet, player);
                if(enemy.CD >= 0) {
                    bullet.isActive = true;
                    updateEnemyProjectile(enemy,delta_time);
                }

            }


            // Cooldown logic for attacks
            if (enemy.CD > 0.0f) {
                enemy.CD -= delta_time;
            } else if (!enemy.isMoving && enemy.id == 1) {  // Reset bee movement
                enemy.isAtPointA = !enemy.isAtPointA;
                enemy.isMoving = true;
            }
            

            // Set animation row based on direction
            Vector2 playerDirection = enemyDirection(enemy.position,player.position);

            if(enemy.id == 0) { // slime
                enemy.frameRect.y = (playerDirection.x > 0) ? enemy.frameHeight * 2 : enemy.frameHeight * 3;
                enemy.frameRect.x = currentFrame * enemy.frameWidth;
            }

            if (enemy.id == 1) {  // Bee 
                enemy.frameRect.y = (playerDirection.x > 0) ? enemy.frameHeight * 1 : enemy.frameHeight * 3;
                if(enemy.isMoving) {
                    enemy.frameRect.x = currentFrame * enemy.frameWidth;
                }
            }

            if(enemy.id == 2) { // Ghost
                enemy.frameRect.y = (playerDirection.x > 0) ? enemy.frameHeight * 1 : enemy.frameHeight * 3;
                if(enemy.isVulnerable) {
                    enemy.frameRect.x = enemy.frameWidth * 3;
                } else {
                    if(currentFrame < 3){
                        enemy.frameRect.x = currentFrame * enemy.frameWidth;
                    }
                }
                if(enemy.CD <= 0) {
                    enemy.CD = 10;
                    enemy.isVulnerable = false;
                } else {
                    enemy.isVulnerable = (enemy.CD <= 5.0f);
                }
            }
        } 
    }

// Handling Movement and Direction

    Vector2 direction = {0,0};


if (IsKeyDown(KEY_W)) {
    if (player.position.y - player.radius > 90) { // Top boundary check
        player.position.y -= player.speed * delta_time;
    }
    direction.y = -1;
    playerFrameRec.y = frameHeight * 0; 
    bulletFrameRect.y = bulletFrameHeight * 0;
}
if (IsKeyDown(KEY_S)) {
    if (player.position.y + player.radius < screen.window_height- 90) { // Bottom boundary check
        player.position.y += player.speed * delta_time;
    }
    direction.y = 1;
    playerFrameRec.y = frameHeight * 2;
}
if (IsKeyDown(KEY_A)) {
    if (player.position.x - player.radius > 90) { // Left boundary check
        player.position.x -= player.speed * delta_time;
    }
    direction.x = -1;
    playerFrameRec.y = frameHeight * 1;
}
if (IsKeyDown(KEY_D)) {
    if (player.position.x + player.radius < screen.window_width- 90) { // Right boundary check
        player.position.x += player.speed * delta_time;
    }
    direction.x = 1;
    playerFrameRec.y = frameHeight * 3;
}  
// Handling Shooting
    if (shootCounter > 0) {
        shootCounter -= delta_time;
    }


    if (IsKeyPressed(KEY_SPACE) ) {
    spawnPlayerProjectile(player, direction);
    } 

// Update player projectiles
    updatePlayerProjectile(player, delta_time);

// handling skills
    if(IsKeyPressed(KEY_E) && skill.canCast ) {
        skill.isCasting = true;
        skill.radius = 0.0f;
        skill.canCast = false;
        skillCounter = skill.CD;
        castingCounter = skill.duration;
    }
    if(!skill.canCast) {
        skillCounter -= delta_time;
        if(skillCounter<=0) {
            skill.canCast = true;
        }
    }
    if(skill.isCasting) {
        skill.radius += 100.0f * delta_time;
        castingCounter -= delta_time;
        if(castingCounter <= 0) {
            skill.isCasting = false;
        }
    }

// animation Frame 

    // player
    if(direction.x == 0 && direction.y == 0){
        playerFrameRec.x = frameWidth * 0;
    } else {
        playerFrameRec.x = currentFrame * frameWidth;
    }

    // heart
    if(player.HP > 0) {
        int heartFrameIndex = (((player.HP-.1) / 70.0f) * (heartTexture.width / heartFrameWidth));
        if (heartFrameIndex >= heartTexture.width / heartFrameWidth) {
            heartFrameIndex = (heartTexture.width / heartFrameWidth) - 1; 
        }
        heartFrameRect.x = heartFrameIndex * heartFrameWidth;
    } else {
        heartFrameRect.x = 0; 
    }

// enemy movement
    if(player.collisionCD > 0.0f){
        player.collisionCD -= delta_time;
    }

    //debug help
    // std::cout << "GameScene Update called" << std::endl;
    // std::cout << "Using frameWidth: " << frameWidth << ", frameHeight: " << frameHeight << " in Update()" << std::endl;
}


void GameScene::Draw() {
    // std::cout << "Using frameWidth: " << frameWidth << ", frameHeight: " << frameHeight << " in Draw()" << std::endl;

    //BeginDrawing();
    ClearBackground(BLACK);
    DrawTexture(roomBackground, 0, 0, RAYWHITE);

    Rectangle heartDR = destinationRect(100.0, 60.0, heartFrameWidth, heartFrameHeight, 2);
    Vector2 origin {frameWidth/2, frameHeight / 2};
    Vector2 bulletOrigin {(bulletFrameWidth) / 2, (bulletFrameHeight) / 2};
    Vector2 playerOrigin {frameWidth/2, frameHeight/2};

    for ( auto &enemy : activeEnemies) {
        Rectangle enemyDR = destinationRect(enemy.position.x, enemy.position.y, enemy.frameWidth, enemy.frameHeight, 2);

        if (enemy.isAlive) {
            if (enemy.isHit){
                // DrawCircleLinesV(enemy.position, enemy.radius, enemy.enemyColor);  
                DrawTexturePro(enemy.sprite, enemy.frameRect, enemyDR, origin, 0, MAROON);
            } else {
                // DrawCircleLinesV(enemy.position, enemy.radius, enemy.enemyColor);  
                DrawTexturePro(enemy.sprite, enemy.frameRect, enemyDR, origin, 0, RAYWHITE);
            }

        }
            
        for ( auto &Bullet : enemy.enemyProjectile) {
            if (Bullet.isActive) {
                DrawCircleV(Bullet.bulletPosition, Bullet.radius, Bullet.bulletColor);
            }
        }
    }

        for ( auto &Bullet : player.playerProjectile) {
            bulletDR = destinationRect(Bullet.bulletPosition.x, Bullet.bulletPosition.y, bulletFrameWidth, bulletFrameHeight, 2);
            if (Bullet.isActive) {
                DrawCircleV(Bullet.bulletPosition, Bullet.radius, Bullet.bulletColor);
                DrawTexturePro(bulletTexture, bulletFrameRect, bulletDR, bulletOrigin, 0, RED);
            }
        }
    
    
    
    // stat text
    // DrawText(TextFormat("PLAYER HP: %.2f", player.HP), 20, 20, 20, WHITE);
    DrawText(("Wave: " + std::to_string(currentWave)).c_str(), screen.window_width-100, 10, 20, WHITE);
    DrawText(("Total Point: " + std::to_string(gamePoint)).c_str(), screen.window_width-300, 10, 20, WHITE);
    // DrawText(("Remaining Points: " + std::to_string(pointRemaining)).c_str(), screen.window_width-400, screen.window_height -100, 20, WHITE);


    // Draw remaining enemies
    // DrawText(("Enemies Left: " + std::to_string(activeEnemies.size())).c_str(), 10, 40, 20, WHITE);


    Rectangle playerDR = destinationRect(player.position.x, player.position.y, frameWidth, frameHeight, 2);
    
    // collision circles
    DrawCircleLinesV(player.position, player.radius, player.playerColor);
    DrawTexturePro(playerTexture, playerFrameRec, playerDR, origin, 0, RAYWHITE);      
    DrawTexturePro(heartTexture, heartFrameRect, heartDR, playerOrigin, 0, RAYWHITE); 
 
    // skill area
    if(skill.isCasting){
        DrawCircleLinesV(player.position, skill.radius, skill.color);
    }
    
    //debug help
    // std::cout << "GameScene Draw called" << std::endl;
}

//Functions:



float GetRandomFloat(float min, float max) {
    int randomInt = GetRandomValue(0, 1000); // Generate an integer between 0 and 1000
    float normalized = static_cast<float>(randomInt) / 1000.0f; // Normalize to range [0, 1]
    return  min + normalized * (max - min); // Scale to [min, max]
}


Rectangle GameScene::destinationRect(float x, float y, float& width, float& height, float scale) {
    return Rectangle{
        x - ((width) / 2),
        y - ((height) / 2),
        width * scale,
        height * scale
    };
}

Vector2 GameScene::enemyDirection(Vector2& enemy, Vector2& player){
    Vector2 ed;
    ed = Vector2Subtract(enemy, player);
    return ed;
}

void GameScene::handleEnemyPlayerCollision(Player& p, Enemy& e) {
    Vector2 enemyDirection = Vector2Subtract(e.position, p.position);
    float slimeCollission = Vector2Length(enemyDirection);
    float slimeRadiusSum = e.radius + p.radius;

    if (slimeCollission < slimeRadiusSum && p.collisionCD <= 0.0f){
        enemyDirection = Vector2Normalize(enemyDirection);
        p.HP -= e.damage;
        p.collisionCD = 1.0f;
        p.isHit = true;
    }
}

void GameScene::handleProjectileCollision(Bullet& b, Enemy& e)  {
    Vector2 BulletDirection = Vector2Subtract(b.bulletPosition, e.position);
    float collisionDistance = Vector2Length(BulletDirection);
    float radiusSum = e.radius + b.radius;
    
    if(collisionDistance < radiusSum && b.isActive && !e.isVulnerable) {
        b.isActive = false;
        BulletDirection = Vector2Normalize(BulletDirection);
        e.HP -= b.baseDamage;       
        b.bulletPosition = {0};
        e.isHit = true;
        SetSoundPitch(playerCollisionSound, GetRandomFloat(1,1.5));
        PlaySound(playerCollisionSound);

        std::cout << e.HP << std::endl;
    } 
}

void GameScene::handleEnemyProjectileCollision(Bullet& b, Player& p)  {
    Vector2 BulletDirection = Vector2Subtract(b.bulletPosition, p.position);
    float collisionDistance = Vector2Length(BulletDirection);
    float radiusSum = p.radius + b.radius;
    
    if(collisionDistance < radiusSum && b.isActive && p.collisionCD <= 0.0f) {
        b.isActive = false;
        p.HP -= b.baseDamage;   
        p.collisionCD = 1.0f;    
        b.bulletPosition = {1000};
    }
}

GameScene::Enemy GameScene::createEnemy (int& type, Vector2& position){
    Enemy newEnemy;
    newEnemy.position = position;
    newEnemy.id = type;

    switch (type) {
        case 0:  // slime
            newEnemy.radius = 20.0f;
            newEnemy.enemyColor = RAYWHITE;
            newEnemy.speed = 30;
            newEnemy.damage = 10.0f;
            newEnemy.HP = 40.0f;
            newEnemy.isAlive = true;
            newEnemy.cost = 1;
            newEnemy.point = 10;

            newEnemy.sprite = slimeTexture;
            newEnemy.frameWidth = newEnemy.sprite.width / slimeFrame;
            newEnemy.frameHeight = newEnemy.sprite.height / 4;
            break;
        case 1:  // bee
            newEnemy.radius = 20.0f;
            newEnemy.enemyColor = YELLOW;
            newEnemy.speed = 80;
            newEnemy.damage = 10.0f;
            newEnemy.HP = 30.0f;
            newEnemy.isAlive = true;
            newEnemy.CD = 5.0f;
            newEnemy.canAttack = false;
            newEnemy.isAtPointA = true;
            newEnemy.isMoving = true;
            newEnemy.cost = 3;
            newEnemy.point = 20;

            newEnemy.sprite = beeTexture;
            newEnemy.frameWidth = newEnemy.sprite.width / 5;
            newEnemy.frameHeight = newEnemy.sprite.height / 4;
            break;
        case 2:  // ghost
            newEnemy.radius = 20.0f;
            newEnemy.detectionRadius = 100.0f;
            newEnemy.enemyColor = RED;
            newEnemy.speed = 40;
            newEnemy.damage = 10.0f;
            newEnemy.HP = 20.0f;
            newEnemy.isAlive = true;
            newEnemy.CD = 10.0f;
            newEnemy.canAttack = false;
            newEnemy.cost = 4;
            newEnemy.point = 15;

            newEnemy.sprite = ghostTexture;
            newEnemy.frameWidth = newEnemy.sprite.width / 4;
            newEnemy.frameHeight = newEnemy.sprite.height / 4;
            break;
    }
    newEnemy.frameRect = {0, 0, newEnemy.frameWidth, newEnemy.frameHeight };
    return newEnemy;
}

GameScene::Bullet GameScene::createPlayerProjectile(Vector2& position, Vector2& direction) {
    Bullet newProjectile;
    newProjectile.bulletPosition = position;
    newProjectile.bulletDirection = Vector2Normalize(direction);  // Normalize direction
    newProjectile.speed = 600.0f;  // Speed of the projectile
    newProjectile.radius = 25.0f;  // Size of the projectile
    newProjectile.bulletColor = RED;  // Color of the projectile
    newProjectile.isActive = true;
    newProjectile.shootCD = 0.5f;
    newProjectile.baseDamage = 10.0f;
    return newProjectile;
}


GameScene::Bullet GameScene::createEnemyProjectile(int& type, Vector2& position, Vector2& direction) {
    Bullet newProjectile;
    newProjectile.bulletPosition = position;
    newProjectile.bulletDirection = Vector2Normalize(direction);  // Normalize direction
    newProjectile.speed = 300.0f;  // Speed of the projectile
    newProjectile.radius = 10.0f;  // Size of the projectile
    newProjectile.bulletColor = RED;  // Color of the projectile
    newProjectile.isActive = true;
    newProjectile.canMove = true;
    newProjectile.baseDamage = 10.0f;
    return newProjectile;
}


void GameScene::spawnPlayerProjectile(Player& p, Vector2& direction) {

    if(direction.x == 0 && direction.y == 0) {
        p.canShoot = false;
    } else {
        p.canShoot = true;
    }

    if (shootCounter <= 0 && p.canShoot) {
        Bullet newBullet = createPlayerProjectile(p.position, direction);
        p.playerProjectile.push_back(newBullet);
        shootCounter = newBullet.shootCD;
    }
}

void GameScene::updatePlayerProjectile(Player& player, float delta_time) {
    for (auto& bullet : player.playerProjectile) {
        if (bullet.isActive) {
            // Move bullet
            bullet.bulletPosition.x += bullet.bulletDirection.x * bullet.speed * delta_time;
            bullet.bulletPosition.y += bullet.bulletDirection.y * bullet.speed * delta_time;

            // Deactivate bullet if it goes off-screen
            if (bullet.bulletPosition.x < 0 || bullet.bulletPosition.x > 800 ||
                bullet.bulletPosition.y < 0 || bullet.bulletPosition.y > 600) {
                bullet.isActive = false;
            }
        }
    }
    // Remove inactive bullets
    player.playerProjectile.erase(
        std::remove_if(player.playerProjectile.begin(), player.playerProjectile.end(),
                       [](const Bullet& b) { return !b.isActive; }),
        player.playerProjectile.end());
}

void GameScene::updateEnemyProjectile(Enemy& enemy, float delta_time) {
    // Loop through each enemy's projectiles and remove inactive ones
    for (auto &bullet : enemy.enemyProjectile) {
        if (enemy.isAlive) {          
                if (bullet.isActive) {
                    // Update projectile position based on its direction and speed
                    bullet.bulletPosition.x += bullet.bulletDirection.x * bullet.speed * delta_time;
                    bullet.bulletPosition.y += bullet.bulletDirection.y * bullet.speed * delta_time;

                    // Check if the projectile has gone off the screen or hit something
                    if (bullet.bulletPosition.x < 0 || bullet.bulletPosition.x > screen.window_width ||
                        bullet.bulletPosition.y < 0 || bullet.bulletPosition.y > screen.window_height) {
                        bullet.isActive = false;  // Deactivate projectile if out of bounds
                    }
                } 
            enemy.enemyProjectile.erase(
                std::remove_if(enemy.enemyProjectile.begin(), enemy.enemyProjectile.end(),
                               [](const Bullet& b) { return !b.isActive; }),
                enemy.enemyProjectile.end());
        }
    }
}

void GameScene::checkProjectileCollision(Player& player, Enemy& enemy) {
    for (auto& bullet : player.playerProjectile) {
        if (bullet.isActive) {
            for (auto& enemy : activeEnemies) {
                if (enemy.isAlive) { // Only check against alive enemies
                    handleProjectileCollision(bullet, enemy);

                    // Check if the enemy's HP drops to zero
                    if (enemy.HP <= 0) {
                        enemy.isAlive = false;
                        std::cout << "Enemy defeated!\n";
                    }
                    if (enemy.isHit) {
                        enemy.hitTimer -= delta_time;
                    }
                    
                    if (enemy.hitTimer <= 0.0f) {
                            enemy.isHit = false;
                            enemy.hitTimer = 0.1f; // Reset timer
                        }
                    
                }
            }
        }
    }

    // Remove inactive bullets after collision handling
    player.playerProjectile.erase(
    std::remove_if(player.playerProjectile.begin(), player.playerProjectile.end(),
                       [](const Bullet& b) { return !b.isActive; }),
    player.playerProjectile.end());
}


void GameScene::spawnEnemyProjectile(Enemy& e, Vector2& direction) {
    Bullet newBullet = createEnemyProjectile(e.id, e.position, direction);
    e.enemyProjectile.push_back(newBullet);
}

void GameScene::spawnWave(float deltaTime) {
    static float spawnTimer = 0.0f; // Timer to manage spawn intervals
    const float spawnInterval = 1.0f; // 1-second interval between spawns


    // Increment spawn timer by deltaTime
    spawnTimer += deltaTime;

    // Spawn an enemy only if spawnTimer exceeds spawnInterval
    if (spawnTimer >= spawnInterval && pointRemaining > 0) {
        spawnTimer = 0.0f; // Reset the timer

        Vector2 spawnPosition;
        int edge = GetRandomValue(0, 3);

        // Determine spawn position based on random edge
        switch (edge) {
            case 0: spawnPosition = {static_cast<float>(GetRandomValue(0, screen.window_width)), 0}; break;
            case 1: spawnPosition = {screen.window_width, static_cast<float>(GetRandomValue(0, screen.window_height))}; break;
            case 2: spawnPosition = {static_cast<float>(GetRandomValue(0, screen.window_width)), screen.window_height}; break;
            case 3: spawnPosition = {0, static_cast<float>(GetRandomValue(0, screen.window_height))}; break;
        }

        // Create an enemy of a random type
        int type = GetRandomValue(0, 2);
        Enemy newEnemy = createEnemy(type, spawnPosition);
        std::cout << newEnemy.HP << " " << waveTimer << " " << waveActive <<  std::endl;

        // Check if the enemy's cost fits within the remaining points
        if (newEnemy.cost <= pointRemaining) {
            activeEnemies.push_back(newEnemy);
            pointRemaining -= newEnemy.cost;
        }
    }
}



void GameScene::updateWave(float deltaTime) {
    // Check if all enemies are defeated
    bool allEnemiesDefeated = true;
    
    for (const auto& enemy : activeEnemies) {
        if (enemy.isAlive) {
            allEnemiesDefeated = false;
            break;
        }
    }

    if (waveActive) {
        // If the wave is active, spawn enemies for the wave
        spawnWave(deltaTime);

        // If all enemies are defeated, mark the wave as completed
        if (allEnemiesDefeated && pointRemaining <= 0) {
            waveActive = false; // End current wave
            waveTimer = waveCD; // Start cooldown for next wave
        }
    } else {
        // If wave is not active, decrement cooldown timer
        waveTimer -= deltaTime;
        if (waveTimer <= 0.0f) {
            // Prepare for the next wave
            activeEnemies.clear();  // Remove any lingering enemies
            pointRemaining = (currentWave + 1) * 10;     // Reset points to trigger wave initialization
            currentWave++;
            waveActive = true;      // Activate the wave
        }
    }
}

