#include "main_menu_scene-h.hpp"
#include "scene_manager.hpp"
#include "SaveSystem.hpp"
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
    
    // Check if save file exists
    bool has_save = SaveSystem::GetInstance()->HasSaveFile();
    
    // If save exists, add Continue button
    if (has_save) {
        buttons.push_back(new MenuButton(
            {570, 250, 200, 50},
            "Continue",
            RED, 
            MAROON, 
            WHITE, 
            [this]() { 
                SceneManager* sceneManager = GetSceneManager();
                if (sceneManager != nullptr) {
                    // Load saved wave
                    int saved_wave = SaveSystem::GetInstance()->LoadWave();
                    
                    // Create a Level scene with the saved wave
                    Level* level_scene = new Level(saved_wave);
                    level_scene->SetSceneManager(sceneManager);
                    
                    // Register this new level scene
                    sceneManager->UnregisterScene(6); // Remove old level scene
                    sceneManager->RegisterScene(level_scene, 6); // Add new one
                    
                    std::cout << "Continuing from wave " << saved_wave << std::endl;
                    sceneManager->SwitchScene(6);
                    sceneManager->CancelExit();
                }
            }
        ));
        
        // New Game button
        buttons.push_back(new MenuButton(
            {570, 325, 200, 50},
            "New Game",
            RED, 
            MAROON, 
            WHITE, 
            [this]() { 
                SceneManager* sceneManager = GetSceneManager();
                if (sceneManager != nullptr) {
                    // Create a Level scene with wave 1
                    Level* level_scene = new Level(1);
                    level_scene->SetSceneManager(sceneManager);
                    
                    // Register this new level scene
                    sceneManager->UnregisterScene(6); // Remove old level scene
                    sceneManager->RegisterScene(level_scene, 6); // Add new one
                    
                    std::cout << "Starting new game at wave 1" << std::endl;
                    sceneManager->SwitchScene(6);
                    sceneManager->CancelExit();
                }
            }
        ));
        
        // Add remaining buttons with shifted positions
        buttons.push_back(new MenuButton(
            {570, 400, 200, 50},
            "Settings", 
            RED, 
            MAROON, 
            WHITE, 
            [this]() { 
                SceneManager* sceneManager = GetSceneManager();
                if (sceneManager != nullptr) {
                    std::cout << "Attempting to switch to settings scene" << std::endl;
                    sceneManager->SwitchScene(3);
                    sceneManager->CancelExit();
                }
            }
        ));

        buttons.push_back(new MenuButton(
            {570, 475, 200, 50},
            "Leaderboards", 
            RED, 
            MAROON, 
            WHITE, 
            [this]() { 
                SceneManager* sceneManager = GetSceneManager();
                if (sceneManager != nullptr) {
                    std::cout << "Attempting to switch to Leaderboards scene" << std::endl;
                    sceneManager->SwitchScene(5);
                    sceneManager->CancelExit();
                }
            }
        ));

        buttons.push_back(new MenuButton(
            {570, 550, 200, 50},
            "Exit Game", 
            RED, 
            MAROON, 
            WHITE, 
            [this]() { 
                SceneManager* sceneManager = GetSceneManager();
                if (sceneManager != nullptr) {
                    sceneManager->RequestGameExit();
                }
            }
        ));
    } 
    else {
        // Original button layout without Continue
        buttons.push_back(new MenuButton(
            {570, 250, 200, 50},
            "Start",
            RED, 
            MAROON, 
            WHITE, 
            [this]() { 
                SceneManager* sceneManager = GetSceneManager();
                if (sceneManager != nullptr) {
                    std::cout << "Attempting to switch to game scene" << std::endl;
                    sceneManager->SwitchScene(6);
                    sceneManager->CancelExit();
                }
            }
        ));

        buttons.push_back(new MenuButton(
            {570, 325, 200, 50},
            "Settings", 
            RED, 
            MAROON, 
            WHITE, 
            [this]() { 
                SceneManager* sceneManager = GetSceneManager();
                if (sceneManager != nullptr) {
                    std::cout << "Attempting to switch to settings scene" << std::endl;
                    sceneManager->SwitchScene(3);
                    sceneManager->CancelExit();
                }
            }
        ));

        buttons.push_back(new MenuButton(
            {570, 400, 200, 50},
            "Leaderboards", 
            RED, 
            MAROON, 
            WHITE, 
            [this]() { 
                SceneManager* sceneManager = GetSceneManager();
                if (sceneManager != nullptr) {
                    std::cout << "Attempting to switch to Leaderboards scene" << std::endl;
                    sceneManager->SwitchScene(5);
                    sceneManager->CancelExit();
                }
            }
        ));

        buttons.push_back(new MenuButton(
            {570, 475, 200, 50},
            "Exit Game", 
            RED, 
            MAROON, 
            WHITE, 
            [this]() { 
                SceneManager* sceneManager = GetSceneManager();
                if (sceneManager != nullptr) {
                    sceneManager->RequestGameExit();
                }
            }
        ));
    }
}

void MainMenu::Begin() {
    std::cout << "MainMenu::Begin() starting" << std::endl;
    
    // Make sure buttons are initialized
    if (buttons.empty()) {
        InitializeButtons();
    }

    // Safely load music
    try {
        std::cout << "Checking if menu theme is already loaded" << std::endl;
        if (menu_theme.ctxData == nullptr) {
            std::cout << "Loading menu theme music" << std::endl;
            menu_theme = LoadMusicStream("menu_theme.ogg");
        }
        
        if (menu_theme.ctxData != nullptr) {
            std::cout << "Menu theme loaded successfully" << std::endl;
            musicLoaded = true;
            AudioManager::GetInstance()->SetCurrentMusic(menu_theme);
            PlayMusicStream(menu_theme);
        } else {
            std::cerr << "Failed to load menu theme music" << std::endl;
            musicLoaded = false;
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception loading music: " << e.what() << std::endl;
        musicLoaded = false;
    } catch (...) {
        std::cerr << "Unknown exception loading music" << std::endl;
        musicLoaded = false;
    }

    // Safely load background texture
    try {
        std::cout << "Loading background texture" << std::endl;
        TextureData backgroundTextureData = ResourceManager::GetInstance()->GetTexture("back_cave.png");
        backgroundTexture = backgroundTextureData.texture;
        std::cout << "Background texture loaded, ID: " << backgroundTexture.id << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Exception loading background: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception loading background" << std::endl;
    }
    
    std::cout << "MainMenu::Begin() completed" << std::endl;
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

    // Safely iterate through buttons
    for (auto& button : buttons) {
        if (button != nullptr) {
            button->HandleClick(mousePoint);
        }
    }

    // Safely update music
    if (musicLoaded && menu_theme.ctxData != nullptr) {
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
    
    // Only draw the background if the texture is valid
    if (backgroundTexture.id > 0) {
        DrawTexturePro(backgroundTexture, {0, 0, 2000, 2000}, {0,0,1280,720}, {0,0}, 0.0f, WHITE);
    }

    DrawText("MAIN MENU", 450, 100, 80, WHITE);

    for (const auto& button : buttons) {
        if (button != nullptr) {
            button->Draw();
        }
    }
}