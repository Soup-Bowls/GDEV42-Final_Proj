#ifndef SAVE_SYSTEM_HPP
#define SAVE_SYSTEM_HPP

#include <string>
#include <iostream>
#include <fstream>

struct SaveData {
    int wave;
    int playerHealth;
};

class SaveSystem {
public:
    static SaveSystem* GetInstance() {
        static SaveSystem instance;
        return &instance;
    }

    bool SaveGame(int wave, int playerHealth) {
        try {
            std::ofstream saveFile("savegame.txt");
            if (saveFile.is_open()) {
                saveFile << wave << " " << playerHealth;
                saveFile.close();
                std::cout << "Saved wave: " << wave << " with health: " << playerHealth << std::endl;
                return true;
            } else {
                return false;
            }
        } catch (const std::exception& e) {
            return false;
        }
    }

    SaveData LoadGame() {
        SaveData data = {1, 100}; // Default: wave 1, full health
        

        std::ifstream saveFile("savegame.txt");
        if (saveFile.is_open()) {
            saveFile >> data.wave >> data.playerHealth;
            saveFile.close();
            std::cout << "Loaded wave: " << data.wave << " with health: " << data.playerHealth << std::endl;
        } else {
            std::cout << "No save file found" << std::endl;
        }
        
        return data;
    }

    bool HasSaveFile() {
        std::ifstream saveFile("savegame.txt");
        return saveFile.good();
    }

private:
    SaveSystem() {}
    SaveSystem(const SaveSystem&) = delete;
    void operator=(const SaveSystem&) = delete;
};

#endif