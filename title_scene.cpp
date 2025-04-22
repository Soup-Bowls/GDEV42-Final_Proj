#include "title_scene-h.hpp"
#include "scene_manager.hpp"
#include <iostream>

void TitleScene::Begin() {
    TextureData eyeballTextureData = ResourceManager::GetInstance()->GetTexture("eyeball.png");
    eyeball = eyeballTextureData.texture;
    TextureData caveTexture = ResourceManager::GetInstance()->GetTexture("back_cave.png");
    cavebg = caveTexture.texture;

    if (!IsMusicReady(title_theme)) {
        title_theme = LoadMusicStream("title-theme.mp3");
    }
    
    if (IsMusicReady(title_theme)) {
        musicLoaded = true;
        AudioManager::GetInstance()->SetCurrentMusic(title_theme);
        PlayMusicStream(title_theme);
    } else {
        std::cerr << "Failed to load menu theme music" << std::endl;
    }
}

void TitleScene::End() {
    if (IsMusicReady(title_theme)) {
        StopMusicStream(title_theme);
        UnloadMusicStream(title_theme);
        
        title_theme = {0};
        musicLoaded = false;
    }   
}

void TitleScene::Update() {
    if (IsKeyPressed(KEY_ENTER)) {
        if (GetSceneManager() != nullptr) {
            GetSceneManager()->SwitchScene(1);
        }
    }

    if (musicLoaded && IsMusicReady(title_theme)) {
        UpdateMusicStream(title_theme);

        if (!IsMusicStreamPlaying(title_theme)) {
            PlayMusicStream(title_theme);
        }
    }
}

void TitleScene::Draw() {
    ClearBackground(BLACK);
    DrawTexturePro(cavebg, {0, 0, 2000, 2000}, {0,0,1280,720}, {0,0},  0.0f,  WHITE);
    DrawTexturePro(eyeball, {63, 126, 63, 63}, {514, 100, 252, 252}, {0, 0}, 0.0f, WHITE);
    DrawText("DUNGEON DESCENT", 350, 350, 60, RED);
    DrawText("Press ENTER", 550, 450, 30, WHITE);
}