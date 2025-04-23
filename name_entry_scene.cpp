#include "name_entry_scene-h.hpp"
#include <iostream>

NameEntryScene::NameEntryScene() 
    : playerScore(0), playerName("AAA"), maxNameLength(3), selectedChar(0), nameConfirmed(false) {
}

void NameEntryScene::Begin() {
    TextureData bgData = ResourceManager::GetInstance()->GetTexture("back_cave.png");
    background = bgData.texture;
    
    playerName = "AAA";
    selectedChar = 0;
    nameConfirmed = false;
}

void NameEntryScene::End() {
    if (nameConfirmed) {
        highScoreManager.AddHighScore(playerName, playerScore);
    }
}

void NameEntryScene::SetPlayerScore(int score) {
    playerScore = score;
}

void NameEntryScene::HandleKeyboardInput() {
    if (IsKeyPressed(KEY_RIGHT)) {
        selectedChar = (selectedChar + 1) % maxNameLength;
    }
    
    if (IsKeyPressed(KEY_LEFT)) {
        selectedChar = (selectedChar - 1 + maxNameLength) % maxNameLength;
    }
    
    if (IsKeyPressed(KEY_UP)) {
        char& c = playerName[selectedChar];
        c = (c == 'Z') ? 'A' : c + 1;
    }
    
    if (IsKeyPressed(KEY_DOWN)) {
        char& c = playerName[selectedChar];
        c = (c == 'A') ? 'Z' : c - 1;
    }
    
    if (IsKeyPressed(KEY_ENTER)) {
        nameConfirmed = true;
        highScoreManager.AddHighScore(playerName, playerScore);
        
        if (GetSceneManager() != nullptr) {
            GetSceneManager()->SwitchScene(5); // Switch to leaderboard scene
        }
    }
}

void NameEntryScene::Update() {
    if (!nameConfirmed) {
        HandleKeyboardInput();
    }
}

void NameEntryScene::Draw() {
    ClearBackground(BLACK);
    DrawTexturePro(background, {0, 0, 2000, 2000}, {0, 0, 800, 600}, {0, 0}, 0.0f, WHITE);
    
    DrawText("NEW HIGH SCORE!", 200, 100, 50, YELLOW);
    
    DrawText("Your Score:", 250, 180, 30, WHITE);
    DrawText(std::to_string(playerScore).c_str(), 450, 180, 30, WHITE);
    
    DrawText("Enter Your Name:", 200, 250, 30, WHITE);
    
    for (int i = 0; i < maxNameLength; i++) {
        Rectangle charBox = {300.0f + i * 70, 300, 50, 50};
        
        Color boxColor = (i == selectedChar) ? RED : DARKGRAY;
        DrawRectangleRec(charBox, boxColor);
        DrawRectangleLinesEx(charBox, 2, WHITE);
        
        char charStr[2] = {playerName[i], '\0'};
        DrawText(charStr, charBox.x + 15, charBox.y + 10, 40, WHITE);
    }
    
    DrawText("Use arrow keys to select and change characters", 150, 380, 20, WHITE);
    DrawText("Press ENTER to confirm", 280, 420, 20, WHITE);
}
