#include "settings_scene-h.hpp"
#include "scene_manager.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>

void SettingsScene::SaveSettings() {
    std::ofstream configFile("settingsconfig.txt");
    if (configFile.is_open()) {
        configFile << masterVolumeSlider->GetValue() << std::endl;
        
        configFile << musicVolumeSlider->GetValue() << std::endl;
        
        configFile << sfxVolumeSlider->GetValue() << std::endl;
        
        configFile.close();
        std::cout << "Settings saved successfully" << std::endl;
    } else {
        std::cerr << "Unable to open settings file for writing" << std::endl;
    }
}

void SettingsScene::LoadSettings() {
    std::ifstream configFile("settingsconfig.txt");
    if (configFile.is_open()) {
        float masterVol, musicVol, sfxVol;
        
        if (configFile >> masterVol >> musicVol >> sfxVol) {

            masterVol = std::clamp(masterVol, 0.0f, 1.0f);
            musicVol = std::clamp(musicVol, 0.0f, 1.0f);
            sfxVol = std::clamp(sfxVol, 0.0f, 1.0f);
            
            // Update sliders
            masterVolumeSlider->SetValue(masterVol);
            musicVolumeSlider->SetValue(musicVol);
            sfxVolumeSlider->SetValue(sfxVol);
            
            // Update volumes
            AudioManager::GetInstance()->SetMasterVolume(masterVol);
            
            std::cout << "Settings loaded successfully" << std::endl;
        } else {
            std::cerr << "Error reading settings file" << std::endl;
        }
        
        configFile.close();
    } else {
        std::cout << "No existing settings file. Using default values." << std::endl;
    }
}

VolumeSlider::VolumeSlider(Rectangle bounds, const char* label, float initialValue)
    : label(label), currentValue(initialValue) 
{
    this->bounds = bounds;
}

void VolumeSlider::Draw() 
{
    // Draw slider background
    DrawRectangleRec(bounds, sliderColor);

    // Draw slider handle
    float handleWidth = 20;
    Rectangle handle = {
        bounds.x + (bounds.width * currentValue) - handleWidth/2, 
        bounds.y - 5, 
        handleWidth, 
        bounds.height + 10
    };
    DrawRectangleRec(handle, handleColor);

    // Draw label
    DrawText(label, bounds.x, bounds.y - 25, 20, WHITE);

    // Draw current value percentage
    char valueStr[10];
    snprintf(valueStr, sizeof(valueStr), "%.0f%%", currentValue * 100);
    DrawText(valueStr, bounds.x + bounds.width + 10, bounds.y, 20, WHITE);
}

bool VolumeSlider::HandleClick(Vector2 click_position) 
{
    if (CheckCollisionPointRec(click_position, bounds)) {
        // Calculate new value based on mouse x position
        float relativeX = click_position.x - bounds.x;
        currentValue = std::clamp(relativeX / bounds.width, 0.0f, 1.0f);
        return true;
    }
    return false;
}

SettingsScene::SettingsScene() {
    InitializeUI();
}

void SettingsScene::InitializeUI() {
    // Clear existing UI elements
    for (auto element : uiElements) {
        delete element;
    }
    uiElements.clear();

    // Master Volume Slider
    masterVolumeSlider = new VolumeSlider(
        {500, 200, 300, 20}, 
        "Master Volume", 
        0.5f  // Default value
    );
    uiElements.push_back(masterVolumeSlider);

    // Music Volume Slider
    musicVolumeSlider = new VolumeSlider(
        {500, 300, 300, 20}, 
        "Music Volume", 
        0.5f  // Default value, can be expanded to track music volume separately
    );
    uiElements.push_back(musicVolumeSlider);

    // SFX Volume Slider
    sfxVolumeSlider = new VolumeSlider(
        {500, 400, 300, 20}, 
        "SFX Volume", 
        0.5f  // Default value, can be expanded to track SFX volume separately
    );
    uiElements.push_back(sfxVolumeSlider);

    LoadSettings();

    // Back to Menu Button
    buttons.push_back(new MenuButton(
        {500, 500, 200, 50},
        "Back to Menu", 
        RED, 
        MAROON, 
        WHITE, 
        [this]() { 
            SceneManager* sceneManager = GetSceneManager();
            if (sceneManager != nullptr) {
                std::cout << "Returning to Main Menu" << std::endl;
                sceneManager->SwitchScene(1);
                sceneManager->CancelExit();
            }
        }
    ));
    uiElements.push_back(buttons.back());
}

void SettingsScene::Begin() {

    if (uiElements.empty()) {
        InitializeUI();
    }

    // Load background texture
    TextureData settingsBackgroundData = ResourceManager::GetInstance()->GetTexture("back_cave.png");
    settingsBackground = settingsBackgroundData.texture;

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
}

void SettingsScene::End() {
    SaveSettings();

    if (IsMusicReady(menu_theme)) {
        StopMusicStream(menu_theme);
        UnloadMusicStream(menu_theme);
        
        menu_theme = {0};
        musicLoaded = false;
    }
}

SettingsScene::~SettingsScene() {
    for (auto element : uiElements) {
        delete element;
    }
    uiElements.clear();
    buttons.clear();
}

void SettingsScene::Update() {
    Vector2 mousePoint = GetMousePosition();

    // Handle mouse input for sliders and buttons
    for (auto& element : uiElements) {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            element->HandleClick(mousePoint);
        }
    }

    // Update volumes based on slider values
    UpdateVolumes();

    if (musicLoaded && IsMusicReady(menu_theme)) {
        UpdateMusicStream(menu_theme);

    if (!IsMusicStreamPlaying(menu_theme)) {
            PlayMusicStream(menu_theme);
        }
    }
}

void SettingsScene::UpdateVolumes() {
    // Update master volume
    AudioManager::GetInstance()->SetMasterVolume(masterVolumeSlider->GetValue());

    // Note: In a more advanced implementation, you'd separately track 
    // music and SFX volumes
}

void SettingsScene::Draw() {
    ClearBackground(DARKGRAY);
    
    // Draw background
    DrawTexturePro(settingsBackground, {0, 0, 2000, 2000}, {0,0,1280,720}, {0,0},  0.0f,  WHITE);

    // Draw title
    DrawText("SETTINGS", 520, 100, 50, WHITE);

    // Draw all UI elements (sliders and buttons)
    for (const auto& element : uiElements) {
        element->Draw();
    }
}