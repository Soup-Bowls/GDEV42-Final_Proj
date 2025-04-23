#ifndef HIGH_SCORE_HPP
#define HIGH_SCORE_HPP

#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <iostream>

#define MAX_HIGH_SCORES 10

struct HighScoreEntry {
    std::string playerName;
    int score;
    
    HighScoreEntry(const std::string& name = "???", int scoreValue = 0) 
        : playerName(name), score(scoreValue) {}
        
    bool operator<(const HighScoreEntry& other) const {
        return score > other.score;
    }
};

class HighScoreManager {
private:
    int highScore;
    std::string fileName;
    
public:
    HighScoreManager(const std::string& file = "highscore.dat") 
        : highScore(0), fileName(file) {
        LoadHighScore();
    }
    
    void LoadHighScore() {
        std::ifstream file(fileName);
        if (file.is_open()) {
            file >> highScore;
            file.close();
            std::cout << "Loaded high score: " << highScore << std::endl;
        } else {
            std::cout << "No high score file found. Starting with 0." << std::endl;
            highScore = 0;
        }
    }
    
    void SaveHighScore() {
        std::ofstream file(fileName);
        if (file.is_open()) {
            file << highScore;
            file.close();
            std::cout << "Saved high score: " << highScore << std::endl;
        } else {
            std::cerr << "Error: Could not save high score to file." << std::endl;
        }
    }
    
    bool IsHighScore(int score) const {
        return score > highScore;
    }
    
    void UpdateHighScore(int score) {
        if (score > highScore) {
            highScore = score;
            SaveHighScore();
        }
    }
    
    int GetHighScore() const {
        return highScore;
    }
};
