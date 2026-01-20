#include "lessons/LessonManager.hpp"
#include "gui/Colors.hpp"

namespace lessons {

LessonManager::LessonManager() {
    // Alle Lektionen erstellen
    lessons_.push_back(std::make_unique<Lesson1_Basics>());
    lessons_.push_back(std::make_unique<Lesson2_Creation>());
    lessons_.push_back(std::make_unique<Lesson3_Operations>());
    lessons_.push_back(std::make_unique<Lesson4_Database>());

    // Fortschritt initialisieren
    progress_.resize(lessons_.size());
}

void LessonManager::update(float deltaTime) {
    if (inLesson_ && currentLesson_ >= 0 && currentLesson_ < static_cast<int>(lessons_.size())) {
        lessons_[currentLesson_]->update(deltaTime);

        // Fortschritt aktualisieren
        auto& lesson = lessons_[currentLesson_];
        auto& prog = progress_[currentLesson_];
        prog.lastStep = lesson->getCurrentStepIndex();
        prog.completionPercentage = static_cast<float>(prog.lastStep + 1) / lesson->getTotalSteps();
        if (lesson->isComplete()) {
            prog.completed = true;
        }
    }
}

void LessonManager::render() {
    if (inLesson_) {
        renderCurrentLesson();
    } else {
        renderLessonSelection();
    }
}

void LessonManager::selectLesson(int index) {
    if (index >= 0 && index < static_cast<int>(lessons_.size())) {
        currentLesson_ = index;
        inLesson_ = true;
        lessons_[index]->init();
    }
}

void LessonManager::exitCurrentLesson() {
    if (currentLesson_ >= 0 && currentLesson_ < static_cast<int>(lessons_.size())) {
        lessons_[currentLesson_]->cleanup();
    }
    inLesson_ = false;
    currentLesson_ = -1;
}

void LessonManager::renderLessonSelection() {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    // Titel
    const char* title = "Waehle eine Lektion";
    int titleWidth = MeasureText(title, 32);
    DrawText(title, (screenWidth - titleWidth) / 2, 90, 32, gui::Colors::TEXT_HIGHLIGHT);

    // Lektionskarten
    float cardWidth = 350;
    float cardHeight = 200;
    float spacing = 30;
    float totalWidth = lessons_.size() * cardWidth + (lessons_.size() - 1) * spacing;
    float startX = (screenWidth - totalWidth) / 2;
    float startY = 180;

    // Wenn zu viele Lektionen, in Zeilen anordnen
    int cardsPerRow = 2;
    if (lessons_.size() > 4) cardsPerRow = 3;

    for (size_t i = 0; i < lessons_.size(); ++i) {
        int row = static_cast<int>(i) / cardsPerRow;
        int col = static_cast<int>(i) % cardsPerRow;

        float x = startX + col * (cardWidth + spacing);
        float y = startY + row * (cardHeight + spacing);

        // Zentrierung für 2x2 Grid
        if (lessons_.size() == 4) {
            x = (screenWidth - 2 * cardWidth - spacing) / 2 + col * (cardWidth + spacing);
        }

        Rectangle cardBounds = {x, y, cardWidth, cardHeight};

        // Karte zeichnen
        bool hover = CheckCollisionPointRec(GetMousePosition(), cardBounds);
        bool completed = progress_[i].completed;

        Color borderColor = completed ? gui::Colors::ACCENT_GREEN : gui::Colors::PRIMARY;
        if (hover) {
            DrawRectangleRounded(cardBounds, 0.05f, 4, gui::Colors::BUTTON_HOVER);
        } else {
            DrawRectangleRounded(cardBounds, 0.05f, 4, gui::Colors::BACKGROUND_PANEL);
        }
        DrawRectangleRoundedLines(cardBounds, 0.05f, 4, 2, borderColor);

        // Lektionsnummer
        DrawText(TextFormat("Lektion %zu", i + 1), static_cast<int>(x + 20), static_cast<int>(y + 15), 16, gui::Colors::TEXT_DIM);

        // Titel
        DrawText(lessons_[i]->getTitle().c_str(), static_cast<int>(x + 20), static_cast<int>(y + 40), 24, gui::Colors::TEXT_HIGHLIGHT);

        // Beschreibung
        gui::UIComponents::paragraph({x + 20, y + 75, cardWidth - 40, 80}, lessons_[i]->getDescription(), 16);

        // Fortschrittsbalken
        if (progress_[i].completionPercentage > 0) {
            Rectangle progBounds = {x + 20, y + cardHeight - 35, cardWidth - 40, 8};
            DrawRectangleRounded(progBounds, 0.5f, 4, gui::Colors::BACKGROUND_LIGHT);
            Rectangle fillBounds = {progBounds.x, progBounds.y, progBounds.width * progress_[i].completionPercentage, progBounds.height};
            DrawRectangleRounded(fillBounds, 0.5f, 4, completed ? gui::Colors::ACCENT_GREEN : gui::Colors::PRIMARY);
        }

        // Status
        if (completed) {
            DrawText("Abgeschlossen", static_cast<int>(x + cardWidth - 120), static_cast<int>(y + cardHeight - 30), 14, gui::Colors::ACCENT_GREEN);
        } else if (progress_[i].completionPercentage > 0) {
            DrawText(TextFormat("%.0f%%", progress_[i].completionPercentage * 100), static_cast<int>(x + cardWidth - 50), static_cast<int>(y + cardHeight - 30), 14, gui::Colors::PRIMARY);
        }

        // Klick-Handler
        if (hover && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            selectLesson(static_cast<int>(i));
        }
    }

    // Hinweis
    const char* hint = "Klicke auf eine Karte, um die Lektion zu starten";
    int hintWidth = MeasureText(hint, 16);
    DrawText(hint, (screenWidth - hintWidth) / 2, screenHeight - 100, 16, gui::Colors::TEXT_DIM);
}

void LessonManager::renderCurrentLesson() {
    if (currentLesson_ >= 0 && currentLesson_ < static_cast<int>(lessons_.size())) {
        lessons_[currentLesson_]->render();

        // Zurück-Button (zusätzlich zum Header)
        if (gui::UIComponents::button({static_cast<float>(GetScreenWidth()) - 150.0f, 70, 130, 35}, "Beenden")) {
            exitCurrentLesson();
        }
    }
}

} // namespace lessons
