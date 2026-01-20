#pragma once

#include "raylib.h"
#include "gui/Colors.hpp"
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <chrono>

namespace progress {

/**
 * @brief Achievement-Kategorien
 */
enum class AchievementCategory {
    Learning,      // Lektionen abschließen
    Mastery,       // Quiz bestehen
    Explorer,      // Features entdecken
    Creator,       // Tensoren erstellen
    Expert,        // Fortgeschrittene Operationen
    Collector,     // Datenbank-bezogen
    Special        // Besondere Achievements
};

/**
 * @brief Schwierigkeitsgrade für Achievements
 */
enum class AchievementTier {
    Bronze,    // Einfach
    Silver,    // Mittel
    Gold,      // Schwer
    Platinum,  // Sehr schwer
    Diamond    // Legendär
};

/**
 * @brief Ein einzelnes Achievement
 */
struct Achievement {
    std::string id;
    std::string name;
    std::string description;
    std::string hint;  // Geheimer Hinweis
    AchievementCategory category;
    AchievementTier tier;
    int points;
    bool secret;  // Verstecktes Achievement

    // Unlock-Status
    bool unlocked = false;
    std::chrono::system_clock::time_point unlockTime;

    // Fortschritt (für progressive Achievements)
    int currentProgress = 0;
    int requiredProgress = 1;

    // Icon (Index in Spritesheet)
    int iconIndex = 0;
};

/**
 * @brief Statistiken für Achievement-Tracking
 */
struct PlayerStats {
    // Lektionen
    int lessonsCompleted = 0;
    int lessonsStarted = 0;
    int totalLessonSteps = 0;

    // Quizze
    int quizzesPassed = 0;
    int quizzesFailed = 0;
    int perfectQuizzes = 0;  // 100%
    int totalQuizPoints = 0;

    // Sandbox
    int tensorsCreated = 0;
    int operationsPerformed = 0;
    int commandsExecuted = 0;

    // Datenbank
    int tensorsSaved = 0;
    int tensorsLoaded = 0;
    int databaseQueries = 0;

    // Visualisierung
    int visualizationModesUsed = 0;
    int cameraRotations = 0;

    // Zeit
    float totalPlayTime = 0.0f;
    int sessionsPlayed = 0;
    int consecutiveDays = 0;

    // Spezifische Aktionen
    int matmulOperations = 0;
    int transposeOperations = 0;
    int reshapeOperations = 0;
    int largest3DTensorSize = 0;
    int highestTensorRank = 0;
};

/**
 * @brief Achievement-System Manager
 */
class AchievementManager {
public:
    AchievementManager();

    // Initialisierung
    void init();

    // Statistik-Updates
    void recordLessonComplete(const std::string& lessonId);
    void recordLessonStep();
    void recordQuizResult(bool passed, int score, int maxScore);
    void recordTensorCreated(size_t rank, size_t size);
    void recordOperation(const std::string& operation);
    void recordCommand();
    void recordDatabaseAction(const std::string& action);
    void recordVisualizationMode(int mode);
    void updatePlayTime(float deltaTime);

    // Achievement-Check
    void checkAchievements();
    void unlockAchievement(const std::string& id);
    bool isUnlocked(const std::string& id) const;
    float getProgress(const std::string& id) const;

    // Abfragen
    std::vector<Achievement> getAllAchievements() const;
    std::vector<Achievement> getUnlockedAchievements() const;
    std::vector<Achievement> getLockedAchievements() const;
    std::vector<Achievement> getAchievementsByCategory(AchievementCategory category) const;

    int getTotalPoints() const;
    int getUnlockedCount() const;
    float getCompletionPercentage() const;

    // Benachrichtigungen
    bool hasNewUnlock() const { return !recentUnlocks_.empty(); }
    Achievement popRecentUnlock();

    // Persistenz
    void saveProgress(const std::string& filename);
    void loadProgress(const std::string& filename);

    // Statistiken
    const PlayerStats& getStats() const { return stats_; }

    // Rendering
    void renderNotification(float deltaTime);
    void renderAchievementList(Rectangle bounds);
    void renderAchievementPopup(const Achievement& achievement);

private:
    void registerAchievements();
    void registerLearningAchievements();
    void registerMasteryAchievements();
    void registerExplorerAchievements();
    void registerCreatorAchievements();
    void registerExpertAchievements();
    void registerCollectorAchievements();
    void registerSpecialAchievements();

    Color getTierColor(AchievementTier tier) const;
    const char* getTierName(AchievementTier tier) const;
    const char* getCategoryName(AchievementCategory category) const;

    std::map<std::string, Achievement> achievements_;
    std::vector<Achievement> recentUnlocks_;
    PlayerStats stats_;

    // Notification Animation
    float notificationTimer_ = 0.0f;
    Achievement* currentNotification_ = nullptr;
};

/**
 * @brief Spieler-Profil mit Achievements und Statistiken
 */
class PlayerProfile {
public:
    std::string name = "Tensor-Lerner";
    int level = 1;
    int experience = 0;
    int totalPoints = 0;

    void addExperience(int xp);
    int getExperienceForNextLevel() const;
    float getLevelProgress() const;

    std::string getRankTitle() const;
};

} // namespace progress
