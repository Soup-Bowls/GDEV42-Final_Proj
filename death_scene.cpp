#include "death_scene-h.hpp"
#include "scene_manager.hpp"
#include <iostream>

DeathScene::DeathScene() {

}

void DeathScene::Begin() {
    TextureData deathBackgroundData = ResourceManager::GetInstance()->GetTexture("deathbackground.png");
    deathbg = deathBackgroundData.texture;

    if (!IsMusicReady(death_theme)) {
        death_theme = LoadMusicStream("death_theme.ogg");
    }
    
    if (IsMusicReady(death_theme)) {
        musicLoaded = true;
        AudioManager::GetInstance()->SetCurrentMusic(death_theme);
        PlayMusicStream(death_theme);
    } else {
        std::cerr << "Failed to load menu theme music" << std::endl;
    }
}

void DeathScene::End() {
    if (IsMusicReady(death_theme)) {
        StopMusicStream(death_theme);
        UnloadMusicStream(death_theme);
        
        death_theme = {0};
        musicLoaded = false;
    }   
}

void DeathScene::Update() {
    if (IsKeyPressed(KEY_ENTER)) {
        if (GetSceneManager() != nullptr) {
            GetSceneManager()->SwitchScene(0);
        }
    }

    if (musicLoaded && IsMusicReady(death_theme)) {
        UpdateMusicStream(death_theme);

        if (!IsMusicStreamPlaying(death_theme)) {
            PlayMusicStream(death_theme);
        }
    }
}

void DeathScene::Draw() {
    ClearBackground(BLACK);
    DrawTexturePro(deathbg, {0, 0, 2000, 2000}, {0,0,800,600}, {0,0},  0.0f,  BLUE);
    DrawText("YOU DIED", 250, 250, 60, RED);
    DrawText("Press ENTER to return", 30, 550, 20, WHITE);
}