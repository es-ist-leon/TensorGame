#include "progress/Achievements.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>

namespace progress {

AchievementManager::AchievementManager() {
    init();
}

void AchievementManager::init() {
    registerAchievements();
}

void AchievementManager::registerAchievements() {
    registerLearningAchievements();
    registerMasteryAchievements();
    registerExplorerAchievements();
    registerCreatorAchievements();
    registerExpertAchievements();
    registerCollectorAchievements();
    registerSpecialAchievements();
}

void AchievementManager::registerLearningAchievements() {
    achievements_["first_lesson"] = {
        "first_lesson", "Erster Schritt", "Schließe deine erste Lektion ab",
        "", AchievementCategory::Learning, AchievementTier::Bronze, 10, false
    };

    achievements_["lesson_basics"] = {
        "lesson_basics", "Grundlagen gemeistert", "Schließe die Tensor-Grundlagen Lektion ab",
        "", AchievementCategory::Learning, AchievementTier::Bronze, 15, false
    };

    achievements_["lesson_creation"] = {
        "lesson_creation", "Tensor-Schöpfer", "Schließe die Tensor-Erstellung Lektion ab",
        "", AchievementCategory::Learning, AchievementTier::Bronze, 15, false
    };

    achievements_["lesson_operations"] = {
        "lesson_operations", "Operator", "Schließe die Tensor-Operationen Lektion ab",
        "", AchievementCategory::Learning, AchievementTier::Silver, 20, false
    };

    achievements_["lesson_database"] = {
        "lesson_database", "Datenbank-Kenner", "Schließe die Datenbank Lektion ab",
        "", AchievementCategory::Learning, AchievementTier::Silver, 20, false
    };

    achievements_["lesson_broadcasting"] = {
        "lesson_broadcasting", "Broadcasting-Meister", "Schließe die Broadcasting Lektion ab",
        "", AchievementCategory::Learning, AchievementTier::Gold, 25, false
    };

    achievements_["lesson_neural"] = {
        "lesson_neural", "KI-Pionier", "Schließe die Neural Network Lektion ab",
        "", AchievementCategory::Learning, AchievementTier::Gold, 30, false
    };

    achievements_["all_lessons"] = {
        "all_lessons", "Gelehrter", "Schließe alle Lektionen ab",
        "", AchievementCategory::Learning, AchievementTier::Platinum, 100, false,
        {}, 0, 6
    };

    achievements_["lesson_speedrun"] = {
        "lesson_speedrun", "Schnelllerner", "Schließe eine Lektion in unter 5 Minuten ab",
        "", AchievementCategory::Learning, AchievementTier::Silver, 25, true
    };
}

void AchievementManager::registerMasteryAchievements() {
    achievements_["first_quiz"] = {
        "first_quiz", "Quiz-Debütant", "Bestehe dein erstes Quiz",
        "", AchievementCategory::Mastery, AchievementTier::Bronze, 15, false
    };

    achievements_["quiz_perfect"] = {
        "quiz_perfect", "Perfektionist", "Erreiche 100% in einem Quiz",
        "", AchievementCategory::Mastery, AchievementTier::Gold, 50, false
    };

    achievements_["quiz_streak"] = {
        "quiz_streak", "Auf Siegeskurs", "Bestehe 3 Quizze hintereinander",
        "", AchievementCategory::Mastery, AchievementTier::Silver, 30, false,
        {}, 0, 3
    };

    achievements_["quiz_master"] = {
        "quiz_master", "Quiz-Meister", "Bestehe alle Quizze",
        "", AchievementCategory::Mastery, AchievementTier::Platinum, 100, false,
        {}, 0, 5
    };

    achievements_["no_hints"] = {
        "no_hints", "Eigenständig", "Bestehe ein Quiz ohne Hinweise zu nutzen",
        "", AchievementCategory::Mastery, AchievementTier::Silver, 25, false
    };

    achievements_["points_100"] = {
        "points_100", "Punktesammler", "Sammle 100 Quiz-Punkte",
        "", AchievementCategory::Mastery, AchievementTier::Bronze, 20, false,
        {}, 0, 100
    };

    achievements_["points_500"] = {
        "points_500", "Punktejäger", "Sammle 500 Quiz-Punkte",
        "", AchievementCategory::Mastery, AchievementTier::Gold, 50, false,
        {}, 0, 500
    };
}

void AchievementManager::registerExplorerAchievements() {
    achievements_["first_sandbox"] = {
        "first_sandbox", "Sandkasten-Kind", "Öffne zum ersten Mal die Sandbox",
        "", AchievementCategory::Explorer, AchievementTier::Bronze, 5, false
    };

    achievements_["all_viz_modes"] = {
        "all_viz_modes", "Visueller Künstler", "Probiere alle Visualisierungsmodi aus",
        "", AchievementCategory::Explorer, AchievementTier::Silver, 25, false,
        {}, 0, 7
    };

    achievements_["camera_master"] = {
        "camera_master", "Kamera-Profi", "Drehe die Kamera 360 Grad",
        "", AchievementCategory::Explorer, AchievementTier::Bronze, 10, true
    };

    achievements_["keyboard_shortcuts"] = {
        "keyboard_shortcuts", "Tastatur-Ninja", "Nutze 5 verschiedene Tastaturkürzel",
        "", AchievementCategory::Explorer, AchievementTier::Bronze, 15, false,
        {}, 0, 5
    };

    achievements_["settings_explorer"] = {
        "settings_explorer", "Einstellungs-Forscher", "Öffne die Einstellungen",
        "", AchievementCategory::Explorer, AchievementTier::Bronze, 5, false
    };

    achievements_["night_owl"] = {
        "night_owl", "Nachteule", "Spiele nach Mitternacht",
        "", AchievementCategory::Explorer, AchievementTier::Bronze, 10, true
    };
}

void AchievementManager::registerCreatorAchievements() {
    achievements_["first_tensor"] = {
        "first_tensor", "Tensor-Anfänger", "Erstelle deinen ersten Tensor",
        "", AchievementCategory::Creator, AchievementTier::Bronze, 10, false
    };

    achievements_["tensor_10"] = {
        "tensor_10", "Tensor-Fabrik", "Erstelle 10 Tensoren",
        "", AchievementCategory::Creator, AchievementTier::Bronze, 15, false,
        {}, 0, 10
    };

    achievements_["tensor_100"] = {
        "tensor_100", "Tensor-Industrie", "Erstelle 100 Tensoren",
        "", AchievementCategory::Creator, AchievementTier::Silver, 30, false,
        {}, 0, 100
    };

    achievements_["big_tensor"] = {
        "big_tensor", "Groß denken", "Erstelle einen Tensor mit über 1000 Elementen",
        "", AchievementCategory::Creator, AchievementTier::Silver, 25, false
    };

    achievements_["high_rank"] = {
        "high_rank", "Dimensions-Reisender", "Erstelle einen Tensor mit Rang 4 oder höher",
        "", AchievementCategory::Creator, AchievementTier::Gold, 35, false
    };

    achievements_["all_creation_methods"] = {
        "all_creation_methods", "Methodenmeister", "Nutze alle Tensor-Erstellungsmethoden",
        "", AchievementCategory::Creator, AchievementTier::Gold, 40, false,
        {}, 0, 6
    };
}

void AchievementManager::registerExpertAchievements() {
    achievements_["first_matmul"] = {
        "first_matmul", "Matrix-Multiplikator", "Führe deine erste Matrixmultiplikation durch",
        "", AchievementCategory::Expert, AchievementTier::Bronze, 15, false
    };

    achievements_["matmul_master"] = {
        "matmul_master", "Matmul-Meister", "Führe 50 Matrixmultiplikationen durch",
        "", AchievementCategory::Expert, AchievementTier::Gold, 40, false,
        {}, 0, 50
    };

    achievements_["reshape_expert"] = {
        "reshape_expert", "Form-Wandler", "Führe 20 Reshape-Operationen durch",
        "", AchievementCategory::Expert, AchievementTier::Silver, 25, false,
        {}, 0, 20
    };

    achievements_["transpose_expert"] = {
        "transpose_expert", "Transponier-Experte", "Führe 20 Transpose-Operationen durch",
        "", AchievementCategory::Expert, AchievementTier::Silver, 25, false,
        {}, 0, 20
    };

    achievements_["operation_100"] = {
        "operation_100", "Operations-Veteran", "Führe 100 Tensor-Operationen durch",
        "", AchievementCategory::Expert, AchievementTier::Silver, 30, false,
        {}, 0, 100
    };

    achievements_["operation_1000"] = {
        "operation_1000", "Operations-Legende", "Führe 1000 Tensor-Operationen durch",
        "", AchievementCategory::Expert, AchievementTier::Platinum, 75, false,
        {}, 0, 1000
    };

    achievements_["console_power_user"] = {
        "console_power_user", "Konsolen-Profi", "Führe 50 Konsolen-Befehle aus",
        "", AchievementCategory::Expert, AchievementTier::Gold, 35, false,
        {}, 0, 50
    };
}

void AchievementManager::registerCollectorAchievements() {
    achievements_["first_save"] = {
        "first_save", "Daten-Archivar", "Speichere deinen ersten Tensor in der Datenbank",
        "", AchievementCategory::Collector, AchievementTier::Bronze, 10, false
    };

    achievements_["database_10"] = {
        "database_10", "Sammler", "Speichere 10 Tensoren in der Datenbank",
        "", AchievementCategory::Collector, AchievementTier::Silver, 25, false,
        {}, 0, 10
    };

    achievements_["database_50"] = {
        "database_50", "Kurator", "Speichere 50 Tensoren in der Datenbank",
        "", AchievementCategory::Collector, AchievementTier::Gold, 45, false,
        {}, 0, 50
    };

    achievements_["tag_master"] = {
        "tag_master", "Tag-Meister", "Nutze Tags für deine Tensoren",
        "", AchievementCategory::Collector, AchievementTier::Bronze, 15, false
    };

    achievements_["query_expert"] = {
        "query_expert", "Abfrage-Experte", "Führe 20 Datenbank-Abfragen durch",
        "", AchievementCategory::Collector, AchievementTier::Silver, 25, false,
        {}, 0, 20
    };
}

void AchievementManager::registerSpecialAchievements() {
    achievements_["dedication"] = {
        "dedication", "Hingabe", "Spiele an 7 aufeinanderfolgenden Tagen",
        "", AchievementCategory::Special, AchievementTier::Gold, 50, false,
        {}, 0, 7
    };

    achievements_["play_time_1h"] = {
        "play_time_1h", "Zeit-Investierer", "Spiele insgesamt 1 Stunde",
        "", AchievementCategory::Special, AchievementTier::Bronze, 15, false
    };

    achievements_["play_time_10h"] = {
        "play_time_10h", "Tensor-Enthusiast", "Spiele insgesamt 10 Stunden",
        "", AchievementCategory::Special, AchievementTier::Gold, 50, false
    };

    achievements_["completionist"] = {
        "completionist", "Komplettist", "Schalte alle anderen Achievements frei",
        "Fast geschafft!", AchievementCategory::Special, AchievementTier::Diamond, 200, false
    };

    achievements_["easter_egg"] = {
        "easter_egg", "Osterei-Finder", "Finde das versteckte Easter Egg",
        "Versuche 42 irgendwo einzugeben...", AchievementCategory::Special, AchievementTier::Silver, 25, true
    };

    achievements_["tensor_42"] = {
        "tensor_42", "Die Antwort", "Erstelle einen Tensor mit genau 42 Elementen",
        "", AchievementCategory::Special, AchievementTier::Silver, 30, true
    };
}

void AchievementManager::recordLessonComplete(const std::string& lessonId) {
    stats_.lessonsCompleted++;

    // Spezifische Achievements
    if (!isUnlocked("first_lesson")) {
        unlockAchievement("first_lesson");
    }

    if (lessonId == "basics" && !isUnlocked("lesson_basics")) {
        unlockAchievement("lesson_basics");
    }
    if (lessonId == "creation" && !isUnlocked("lesson_creation")) {
        unlockAchievement("lesson_creation");
    }
    if (lessonId == "operations" && !isUnlocked("lesson_operations")) {
        unlockAchievement("lesson_operations");
    }
    if (lessonId == "database" && !isUnlocked("lesson_database")) {
        unlockAchievement("lesson_database");
    }
    if (lessonId == "broadcasting" && !isUnlocked("lesson_broadcasting")) {
        unlockAchievement("lesson_broadcasting");
    }
    if (lessonId == "neural" && !isUnlocked("lesson_neural")) {
        unlockAchievement("lesson_neural");
    }

    // Alle Lektionen
    achievements_["all_lessons"].currentProgress = stats_.lessonsCompleted;
    if (stats_.lessonsCompleted >= 6 && !isUnlocked("all_lessons")) {
        unlockAchievement("all_lessons");
    }
}

void AchievementManager::recordLessonStep() {
    stats_.totalLessonSteps++;
}

void AchievementManager::recordQuizResult(bool passed, int score, int maxScore) {
    if (passed) {
        stats_.quizzesPassed++;
        if (!isUnlocked("first_quiz")) {
            unlockAchievement("first_quiz");
        }

        achievements_["quiz_streak"].currentProgress++;
        if (achievements_["quiz_streak"].currentProgress >= 3 && !isUnlocked("quiz_streak")) {
            unlockAchievement("quiz_streak");
        }
    } else {
        stats_.quizzesFailed++;
        achievements_["quiz_streak"].currentProgress = 0;  // Reset streak
    }

    if (score == maxScore && !isUnlocked("quiz_perfect")) {
        stats_.perfectQuizzes++;
        unlockAchievement("quiz_perfect");
    }

    stats_.totalQuizPoints += score;
    achievements_["points_100"].currentProgress = stats_.totalQuizPoints;
    achievements_["points_500"].currentProgress = stats_.totalQuizPoints;

    if (stats_.totalQuizPoints >= 100 && !isUnlocked("points_100")) {
        unlockAchievement("points_100");
    }
    if (stats_.totalQuizPoints >= 500 && !isUnlocked("points_500")) {
        unlockAchievement("points_500");
    }
}

void AchievementManager::recordTensorCreated(size_t rank, size_t size) {
    stats_.tensorsCreated++;

    if (!isUnlocked("first_tensor")) {
        unlockAchievement("first_tensor");
    }

    achievements_["tensor_10"].currentProgress = stats_.tensorsCreated;
    achievements_["tensor_100"].currentProgress = stats_.tensorsCreated;

    if (stats_.tensorsCreated >= 10 && !isUnlocked("tensor_10")) {
        unlockAchievement("tensor_10");
    }
    if (stats_.tensorsCreated >= 100 && !isUnlocked("tensor_100")) {
        unlockAchievement("tensor_100");
    }

    if (size > 1000 && !isUnlocked("big_tensor")) {
        unlockAchievement("big_tensor");
    }

    if (rank >= 4 && !isUnlocked("high_rank")) {
        unlockAchievement("high_rank");
    }

    if (size == 42 && !isUnlocked("tensor_42")) {
        unlockAchievement("tensor_42");
    }

    if (rank > static_cast<size_t>(stats_.highestTensorRank)) {
        stats_.highestTensorRank = static_cast<int>(rank);
    }
    if (rank == 3 && size > static_cast<size_t>(stats_.largest3DTensorSize)) {
        stats_.largest3DTensorSize = static_cast<int>(size);
    }
}

void AchievementManager::recordOperation(const std::string& operation) {
    stats_.operationsPerformed++;

    achievements_["operation_100"].currentProgress = stats_.operationsPerformed;
    achievements_["operation_1000"].currentProgress = stats_.operationsPerformed;

    if (stats_.operationsPerformed >= 100 && !isUnlocked("operation_100")) {
        unlockAchievement("operation_100");
    }
    if (stats_.operationsPerformed >= 1000 && !isUnlocked("operation_1000")) {
        unlockAchievement("operation_1000");
    }

    if (operation == "matmul") {
        stats_.matmulOperations++;
        if (!isUnlocked("first_matmul")) {
            unlockAchievement("first_matmul");
        }
        achievements_["matmul_master"].currentProgress = stats_.matmulOperations;
        if (stats_.matmulOperations >= 50 && !isUnlocked("matmul_master")) {
            unlockAchievement("matmul_master");
        }
    }

    if (operation == "transpose") {
        stats_.transposeOperations++;
        achievements_["transpose_expert"].currentProgress = stats_.transposeOperations;
        if (stats_.transposeOperations >= 20 && !isUnlocked("transpose_expert")) {
            unlockAchievement("transpose_expert");
        }
    }

    if (operation == "reshape") {
        stats_.reshapeOperations++;
        achievements_["reshape_expert"].currentProgress = stats_.reshapeOperations;
        if (stats_.reshapeOperations >= 20 && !isUnlocked("reshape_expert")) {
            unlockAchievement("reshape_expert");
        }
    }
}

void AchievementManager::recordCommand() {
    stats_.commandsExecuted++;

    achievements_["console_power_user"].currentProgress = stats_.commandsExecuted;
    if (stats_.commandsExecuted >= 50 && !isUnlocked("console_power_user")) {
        unlockAchievement("console_power_user");
    }
}

void AchievementManager::recordDatabaseAction(const std::string& action) {
    stats_.databaseQueries++;

    if (action == "store") {
        stats_.tensorsSaved++;
        if (!isUnlocked("first_save")) {
            unlockAchievement("first_save");
        }

        achievements_["database_10"].currentProgress = stats_.tensorsSaved;
        achievements_["database_50"].currentProgress = stats_.tensorsSaved;

        if (stats_.tensorsSaved >= 10 && !isUnlocked("database_10")) {
            unlockAchievement("database_10");
        }
        if (stats_.tensorsSaved >= 50 && !isUnlocked("database_50")) {
            unlockAchievement("database_50");
        }
    }

    if (action == "load") {
        stats_.tensorsLoaded++;
    }

    if (action == "setTag" && !isUnlocked("tag_master")) {
        unlockAchievement("tag_master");
    }

    achievements_["query_expert"].currentProgress = stats_.databaseQueries;
    if (stats_.databaseQueries >= 20 && !isUnlocked("query_expert")) {
        unlockAchievement("query_expert");
    }
}

void AchievementManager::recordVisualizationMode(int mode) {
    // Bit-Flag für verwendete Modi
    static int usedModes = 0;
    usedModes |= (1 << mode);

    // Zähle gesetzte Bits
    int count = 0;
    for (int i = 0; i < 7; ++i) {
        if (usedModes & (1 << i)) count++;
    }

    stats_.visualizationModesUsed = count;
    achievements_["all_viz_modes"].currentProgress = count;

    if (count >= 7 && !isUnlocked("all_viz_modes")) {
        unlockAchievement("all_viz_modes");
    }
}

void AchievementManager::updatePlayTime(float deltaTime) {
    stats_.totalPlayTime += deltaTime;

    if (stats_.totalPlayTime >= 3600.0f && !isUnlocked("play_time_1h")) {
        unlockAchievement("play_time_1h");
    }
    if (stats_.totalPlayTime >= 36000.0f && !isUnlocked("play_time_10h")) {
        unlockAchievement("play_time_10h");
    }
}

void AchievementManager::checkAchievements() {
    // Prüfe Completionist
    int unlocked = getUnlockedCount();
    int total = static_cast<int>(achievements_.size()) - 1;  // Minus completionist selbst

    if (unlocked >= total && !isUnlocked("completionist")) {
        unlockAchievement("completionist");
    }
}

void AchievementManager::unlockAchievement(const std::string& id) {
    auto it = achievements_.find(id);
    if (it != achievements_.end() && !it->second.unlocked) {
        it->second.unlocked = true;
        it->second.unlockTime = std::chrono::system_clock::now();
        recentUnlocks_.push_back(it->second);
    }
}

bool AchievementManager::isUnlocked(const std::string& id) const {
    auto it = achievements_.find(id);
    return it != achievements_.end() && it->second.unlocked;
}

float AchievementManager::getProgress(const std::string& id) const {
    auto it = achievements_.find(id);
    if (it != achievements_.end()) {
        if (it->second.requiredProgress > 1) {
            return static_cast<float>(it->second.currentProgress) / it->second.requiredProgress;
        }
        return it->second.unlocked ? 1.0f : 0.0f;
    }
    return 0.0f;
}

std::vector<Achievement> AchievementManager::getAllAchievements() const {
    std::vector<Achievement> result;
    for (const auto& [id, achievement] : achievements_) {
        result.push_back(achievement);
    }
    return result;
}

std::vector<Achievement> AchievementManager::getUnlockedAchievements() const {
    std::vector<Achievement> result;
    for (const auto& [id, achievement] : achievements_) {
        if (achievement.unlocked) {
            result.push_back(achievement);
        }
    }
    return result;
}

std::vector<Achievement> AchievementManager::getLockedAchievements() const {
    std::vector<Achievement> result;
    for (const auto& [id, achievement] : achievements_) {
        if (!achievement.unlocked && !achievement.secret) {
            result.push_back(achievement);
        }
    }
    return result;
}

std::vector<Achievement> AchievementManager::getAchievementsByCategory(AchievementCategory category) const {
    std::vector<Achievement> result;
    for (const auto& [id, achievement] : achievements_) {
        if (achievement.category == category) {
            result.push_back(achievement);
        }
    }
    return result;
}

int AchievementManager::getTotalPoints() const {
    int total = 0;
    for (const auto& [id, achievement] : achievements_) {
        if (achievement.unlocked) {
            total += achievement.points;
        }
    }
    return total;
}

int AchievementManager::getUnlockedCount() const {
    int count = 0;
    for (const auto& [id, achievement] : achievements_) {
        if (achievement.unlocked) count++;
    }
    return count;
}

float AchievementManager::getCompletionPercentage() const {
    return static_cast<float>(getUnlockedCount()) / achievements_.size() * 100.0f;
}

Achievement AchievementManager::popRecentUnlock() {
    if (recentUnlocks_.empty()) {
        return {};
    }
    Achievement a = recentUnlocks_.front();
    recentUnlocks_.erase(recentUnlocks_.begin());
    return a;
}

void AchievementManager::saveProgress(const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) return;

    // Stats speichern
    file.write(reinterpret_cast<const char*>(&stats_), sizeof(stats_));

    // Achievements speichern
    size_t count = achievements_.size();
    file.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (const auto& [id, achievement] : achievements_) {
        size_t idLen = id.size();
        file.write(reinterpret_cast<const char*>(&idLen), sizeof(idLen));
        file.write(id.data(), idLen);
        file.write(reinterpret_cast<const char*>(&achievement.unlocked), sizeof(achievement.unlocked));
        file.write(reinterpret_cast<const char*>(&achievement.currentProgress), sizeof(achievement.currentProgress));
    }
}

void AchievementManager::loadProgress(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) return;

    // Stats laden
    file.read(reinterpret_cast<char*>(&stats_), sizeof(stats_));

    // Achievements laden
    size_t count;
    file.read(reinterpret_cast<char*>(&count), sizeof(count));

    for (size_t i = 0; i < count; ++i) {
        size_t idLen;
        file.read(reinterpret_cast<char*>(&idLen), sizeof(idLen));

        std::string id(idLen, '\0');
        file.read(id.data(), idLen);

        bool unlocked;
        int progress;
        file.read(reinterpret_cast<char*>(&unlocked), sizeof(unlocked));
        file.read(reinterpret_cast<char*>(&progress), sizeof(progress));

        auto it = achievements_.find(id);
        if (it != achievements_.end()) {
            it->second.unlocked = unlocked;
            it->second.currentProgress = progress;
        }
    }
}

void AchievementManager::renderNotification(float deltaTime) {
    if (recentUnlocks_.empty()) return;

    if (currentNotification_ == nullptr) {
        currentNotification_ = &recentUnlocks_.front();
        notificationTimer_ = 0.0f;
    }

    notificationTimer_ += deltaTime;

    int screenWidth = GetScreenWidth();

    // Animation
    float slideIn = std::min(1.0f, notificationTimer_ * 3.0f);
    float slideOut = notificationTimer_ > 3.0f ? (notificationTimer_ - 3.0f) * 3.0f : 0.0f;

    float xOffset = (1.0f - slideIn + slideOut) * 400;

    Rectangle notifBounds = {
        static_cast<float>(screenWidth) - 380 + xOffset, 80,
        360, 100
    };

    // Hintergrund mit Tier-Farbe
    Color tierColor = getTierColor(currentNotification_->tier);
    DrawRectangleRounded(notifBounds, 0.1f, 4, {30, 30, 50, 240});
    DrawRectangleRoundedLines(notifBounds, 0.1f, 4, 3, tierColor);

    // "Achievement Unlocked!"
    DrawText("Achievement Freigeschaltet!", static_cast<int>(notifBounds.x + 15),
             static_cast<int>(notifBounds.y + 10), 14, gui::Colors::ACCENT_YELLOW);

    // Name
    DrawText(currentNotification_->name.c_str(), static_cast<int>(notifBounds.x + 15),
             static_cast<int>(notifBounds.y + 30), 22, gui::Colors::TEXT_HIGHLIGHT);

    // Beschreibung
    DrawText(currentNotification_->description.c_str(), static_cast<int>(notifBounds.x + 15),
             static_cast<int>(notifBounds.y + 55), 14, gui::Colors::TEXT_DIM);

    // Punkte
    DrawText(TextFormat("+%d Punkte", currentNotification_->points),
             static_cast<int>(notifBounds.x + notifBounds.width - 100),
             static_cast<int>(notifBounds.y + 75), 16, tierColor);

    // Nach 4 Sekunden entfernen
    if (notificationTimer_ > 4.0f) {
        recentUnlocks_.erase(recentUnlocks_.begin());
        currentNotification_ = nullptr;
    }
}

void AchievementManager::renderAchievementList(Rectangle bounds) {
    gui::UIComponents::panel(bounds, "Achievements");

    float y = bounds.y + 50;
    float itemHeight = 70;

    // Kategorien
    std::vector<AchievementCategory> categories = {
        AchievementCategory::Learning, AchievementCategory::Mastery,
        AchievementCategory::Explorer, AchievementCategory::Creator,
        AchievementCategory::Expert, AchievementCategory::Collector,
        AchievementCategory::Special
    };

    for (auto category : categories) {
        auto achievements = getAchievementsByCategory(category);
        if (achievements.empty()) continue;

        // Kategorie-Header
        DrawText(getCategoryName(category), static_cast<int>(bounds.x + 15),
                 static_cast<int>(y), 18, gui::Colors::PRIMARY_LIGHT);
        y += 30;

        for (const auto& a : achievements) {
            if (y + itemHeight > bounds.y + bounds.height - 20) break;
            if (a.secret && !a.unlocked) continue;  // Versteckte nicht zeigen

            Rectangle itemBounds = {bounds.x + 10, y, bounds.width - 20, itemHeight - 5};

            // Hintergrund
            Color bgColor = a.unlocked ? Color{60, 80, 60, 255} : gui::Colors::BACKGROUND_LIGHT;
            DrawRectangleRounded(itemBounds, 0.1f, 4, bgColor);

            // Tier-Indikator
            Color tierColor = getTierColor(a.tier);
            DrawRectangle(static_cast<int>(itemBounds.x), static_cast<int>(itemBounds.y), 5,
                          static_cast<int>(itemBounds.height), tierColor);

            // Name & Beschreibung
            DrawText(a.name.c_str(), static_cast<int>(itemBounds.x + 15),
                     static_cast<int>(itemBounds.y + 8), 18,
                     a.unlocked ? gui::Colors::TEXT_HIGHLIGHT : gui::Colors::TEXT_DIM);
            DrawText(a.description.c_str(), static_cast<int>(itemBounds.x + 15),
                     static_cast<int>(itemBounds.y + 30), 14, gui::Colors::TEXT_DIM);

            // Fortschritt
            if (!a.unlocked && a.requiredProgress > 1) {
                float progress = static_cast<float>(a.currentProgress) / a.requiredProgress;
                Rectangle progBar = {itemBounds.x + 15, itemBounds.y + 50, 150, 8};
                DrawRectangleRounded(progBar, 0.5f, 4, gui::Colors::BACKGROUND_PANEL);
                progBar.width *= progress;
                DrawRectangleRounded(progBar, 0.5f, 4, gui::Colors::PRIMARY);
                DrawText(TextFormat("%d/%d", a.currentProgress, a.requiredProgress),
                         static_cast<int>(itemBounds.x + 175), static_cast<int>(itemBounds.y + 48),
                         12, gui::Colors::TEXT_DIM);
            }

            // Punkte
            DrawText(TextFormat("%d", a.points),
                     static_cast<int>(itemBounds.x + itemBounds.width - 50),
                     static_cast<int>(itemBounds.y + 20), 20, tierColor);

            y += itemHeight;
        }

        y += 15;  // Abstand zwischen Kategorien
    }

    // Gesamtstatistik unten
    float statY = bounds.y + bounds.height - 60;
    DrawLine(static_cast<int>(bounds.x + 10), static_cast<int>(statY - 10),
             static_cast<int>(bounds.x + bounds.width - 10), static_cast<int>(statY - 10), gui::Colors::BORDER);

    DrawText(TextFormat("Freigeschaltet: %d / %d (%.1f%%)",
                        getUnlockedCount(), static_cast<int>(achievements_.size()), getCompletionPercentage()),
             static_cast<int>(bounds.x + 15), static_cast<int>(statY), 16, gui::Colors::TEXT);
    DrawText(TextFormat("Gesamtpunkte: %d", getTotalPoints()),
             static_cast<int>(bounds.x + bounds.width - 150), static_cast<int>(statY), 16, gui::Colors::ACCENT_YELLOW);
}

Color AchievementManager::getTierColor(AchievementTier tier) const {
    switch (tier) {
        case AchievementTier::Bronze:   return {205, 127, 50, 255};
        case AchievementTier::Silver:   return {192, 192, 192, 255};
        case AchievementTier::Gold:     return {255, 215, 0, 255};
        case AchievementTier::Platinum: return {229, 228, 226, 255};
        case AchievementTier::Diamond:  return {185, 242, 255, 255};
        default: return gui::Colors::TEXT_DIM;
    }
}

const char* AchievementManager::getTierName(AchievementTier tier) const {
    switch (tier) {
        case AchievementTier::Bronze:   return "Bronze";
        case AchievementTier::Silver:   return "Silber";
        case AchievementTier::Gold:     return "Gold";
        case AchievementTier::Platinum: return "Platin";
        case AchievementTier::Diamond:  return "Diamant";
        default: return "Unbekannt";
    }
}

const char* AchievementManager::getCategoryName(AchievementCategory category) const {
    switch (category) {
        case AchievementCategory::Learning:  return "Lernen";
        case AchievementCategory::Mastery:   return "Meisterschaft";
        case AchievementCategory::Explorer:  return "Entdecker";
        case AchievementCategory::Creator:   return "Ersteller";
        case AchievementCategory::Expert:    return "Experte";
        case AchievementCategory::Collector: return "Sammler";
        case AchievementCategory::Special:   return "Besondere";
        default: return "Sonstige";
    }
}

// === PlayerProfile ===

void PlayerProfile::addExperience(int xp) {
    experience += xp;
    while (experience >= getExperienceForNextLevel()) {
        experience -= getExperienceForNextLevel();
        level++;
    }
}

int PlayerProfile::getExperienceForNextLevel() const {
    return level * 100 + 50;  // Einfache Formel
}

float PlayerProfile::getLevelProgress() const {
    return static_cast<float>(experience) / getExperienceForNextLevel();
}

std::string PlayerProfile::getRankTitle() const {
    if (level < 5)  return "Tensor-Anfänger";
    if (level < 10) return "Tensor-Lehrling";
    if (level < 20) return "Tensor-Kenner";
    if (level < 35) return "Tensor-Experte";
    if (level < 50) return "Tensor-Meister";
    if (level < 75) return "Tensor-Großmeister";
    return "Tensor-Legende";
}

} // namespace progress
