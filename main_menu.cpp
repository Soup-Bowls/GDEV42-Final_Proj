#include "main_menu_scene-h.hpp"
#include "scene_manager.hpp"
#include <iostream>

// MenuButton constructor implementation
MenuButton::MenuButton(Rectangle bounds, const char* text, Color normalColor, 
                       Color hoverColor, Color textColor, 
                       std::function<void()> action)
    : text(text), normalColor(normalColor), hoverColor(hoverColor), 
      textColor(textColor), action(action)
{
    this->bounds = bounds;
}

bool MenuButton::HandleClick(Vector2 click_position) 
{
    if (CheckCollisionPointRec(click_position, bounds)) {
        isHovered = true;
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && action) {
            action();
            return true;
        }
    } else {
        isHovered = false;
    }
    return false;
}

MainMenu::MainMenu() {
    InitializeButtons();
}

void MainMenu::InitializeButtons() {
    // Clear existing buttons if any
    for (auto button : buttons) {
        delete button;
    }
    buttons.clear();
    
    // Start Button
    buttons.push_back(new MenuButton(
        {300, 200, 200, 50},
        "Start",
        RED, 
        MAROON, 
        WHITE, 
        [this]() { 
            SceneManager* sceneManager = GetSceneManager();
            if (sceneManager != nullptr) {
                std::cout << "Attempting to switch to game scene" << std::endl;
                sceneManager->SwitchScene(2);
                // exit flag
                sceneManager->CancelExit();
            }
        }
    ));

    // Settings Button
    buttons.push_back(new MenuButton(
        {300, 275, 200, 50},
        "Settings", 
        RED, 
        MAROON, 
        WHITE, 
        [this]() { 
            SceneManager* sceneManager = GetSceneManager();
            if (sceneManager != nullptr) {
                std::cout << "Attempting to switch to settings scene" << std::endl;
                sceneManager->SwitchScene(3);
                // exit flag
                sceneManager->CancelExit();
            }
        }
    ));

    // Achievements Button
    buttons.push_back(new MenuButton(
        {300, 350, 200, 50},
        "Leaderboards", 
        RED, 
        MAROON, 
        WHITE, 
        [this]() { 
            SceneManager* sceneManager = GetSceneManager();
            if (sceneManager != nullptr) {
                std::cout << "Attempting to switch to Leaderboards scene" << std::endl;
                sceneManager->SwitchScene(5);
                // exit flag
                sceneManager->CancelExit();
            }
        }
    ));

    // Exit Button
    buttons.push_back(new MenuButton(
        {300, 425, 200, 50},
        "Exit Game", 
        RED, 
        MAROON, 
        WHITE, 
        [this]() { 
            SceneManager* sceneManager = GetSceneManager();
            if (sceneManager != nullptr) {
                sceneManager->RequestGameExit();
                //add ways to save game state stuff here
            }
        }
    ));
}

void MainMenu::Begin() {
    
    if (buttons.empty()) {
        InitializeButtons();
    }

    if (!IsMusicReady(menu_theme)) {
        menu_theme = LoadMusicStream("menu_theme.ogg");
    }
    
    if (IsMusicReady(menu_theme)) {
        musicLoaded = true;
        AudioManager::GetInstance()->SetCurrentMusic(menu_theme);
        PlayMusicStream(menu_theme);
    } else {
        std::cerr << "Failed to load menu theme music" << std::endl;
    }

    TextureData backgroundTextureData = ResourceManager::GetInstance()->GetTexture("back_cave.png");
    backgroundTexture = backgroundTextureData.texture;
}

void MainMenu::End() {
    if (IsMusicReady(menu_theme)) {
        StopMusicStream(menu_theme);
        UnloadMusicStream(menu_theme);
        
        menu_theme = {0};
        musicLoaded = false;
    }

}

MainMenu::~MainMenu() {
    for (auto button : buttons) {
        delete button;
    }
    buttons.clear();
}

void MainMenu::Update() {
    Vector2 mousePoint = GetMousePosition();

    for (auto& button : buttons) {
        button->HandleClick(mousePoint);
    }

    if (musicLoaded && IsMusicReady(menu_theme)) {
        UpdateMusicStream(menu_theme);

        if (!IsMusicStreamPlaying(menu_theme)) {
            PlayMusicStream(menu_theme);
        }
    }
}

void MenuButton::Draw() 
{
    Color buttonColor = isHovered ? hoverColor : normalColor;
    
    DrawRectangleRec(bounds, buttonColor);
    DrawRectangleLinesEx(bounds, 2, MAROON);
    
    DrawText(text, 
             bounds.x + bounds.width/2 - MeasureText(text, 20)/2, 
             bounds.y + bounds.height/2 - 10, 
             20, 
             textColor);
}

void MainMenu::Draw() {
    ClearBackground(DARKGRAY);
    DrawTexturePro(backgroundTexture, {0, 0, 2000, 2000}, {0,0,800,600}, {0,0},  0.0f,  WHITE);

    DrawText("MAIN MENU", 220, 100, 60, WHITE);

    for (const auto& button : buttons) {
        button->Draw();
    }
}