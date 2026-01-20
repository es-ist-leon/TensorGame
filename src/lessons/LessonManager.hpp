#pragma once

#include "lessons/Lesson.hpp"
#include <memory>
#include <vector>

namespace lessons {

/**
 * @brief Verwaltet alle Lektionen und den Fortschritt
 */
class LessonManager {
public:
    LessonManager();
    ~LessonManager() = default;

    // Update und Render
    void update(float deltaTime);
    void render();

    // Lektionsauswahl
    void selectLesson(int index);
    void exitCurrentLesson();

    // Fortschritt
    struct LessonProgress {
        bool completed = false;
        int lastStep = 0;
        float completionPercentage = 0.0f;
    };

    const std::vector<LessonProgress>& getProgress() const { return progress_; }
    int getTotalLessons() const { return static_cast<int>(lessons_.size()); }

private:
    void renderLessonSelection();
    void renderCurrentLesson();

    std::vector<std::unique_ptr<Lesson>> lessons_;
    std::vector<LessonProgress> progress_;
    int currentLesson_ = -1;  // -1 = Auswahlbildschirm
    bool inLesson_ = false;
};

} // namespace lessons
