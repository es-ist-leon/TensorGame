#pragma once

#include "raylib.h"
#include "gui/UIComponents.hpp"
#include "gui/TensorVisualizer.hpp"
#include "tensor/Tensor.hpp"
#include <string>
#include <vector>
#include <functional>

namespace lessons {

/**
 * @brief Ein einzelner Schritt innerhalb einer Lektion
 */
struct LessonStep {
    std::string title;
    std::string content;
    std::string code;
    std::function<tensor::Tensor()> tensorGenerator;
    gui::VisualizationMode visualMode = gui::VisualizationMode::Cubes;
    bool interactive = false;
    std::string interactiveHint;
};

/**
 * @brief Basis-Klasse für alle Lektionen
 */
class Lesson {
public:
    Lesson(const std::string& title, const std::string& description);
    virtual ~Lesson() = default;

    // Lebenszyklus
    virtual void init();
    virtual void update(float deltaTime);
    virtual void render();
    virtual void cleanup();

    // Navigation
    void nextStep();
    void previousStep();
    bool isComplete() const;
    int getCurrentStepIndex() const { return currentStep_; }
    int getTotalSteps() const { return static_cast<int>(steps_.size()); }

    // Info
    const std::string& getTitle() const { return title_; }
    const std::string& getDescription() const { return description_; }

protected:
    // Schritte hinzufügen
    void addStep(const LessonStep& step);

    // UI-Hilfsfunktionen
    void renderContent();
    void renderVisualization();
    void renderCode();
    void renderNavigation();

    std::string title_;
    std::string description_;
    std::vector<LessonStep> steps_;
    int currentStep_ = 0;

    gui::TensorVisualizer visualizer_;
    gui::TypeWriter typeWriter_;
    float codeProgress_ = 0.0f;
    bool showCode_ = true;

    // Layout
    Rectangle contentBounds_;
    Rectangle vizBounds_;
    Rectangle codeBounds_;
};

/**
 * @brief Lektion 1: Was ist ein Tensor?
 */
class Lesson1_Basics : public Lesson {
public:
    Lesson1_Basics();
    void init() override;
};

/**
 * @brief Lektion 2: Tensoren erstellen
 */
class Lesson2_Creation : public Lesson {
public:
    Lesson2_Creation();
    void init() override;
    void update(float deltaTime) override;

private:
    int userDimension_ = 1;
    std::vector<int> userShape_ = {3};
};

/**
 * @brief Lektion 3: Tensor-Operationen
 */
class Lesson3_Operations : public Lesson {
public:
    Lesson3_Operations();
    void init() override;
    void update(float deltaTime) override;
    void render() override;

private:
    tensor::Tensor tensorA_;
    tensor::Tensor tensorB_;
    tensor::Tensor result_;
    float operationProgress_ = 0.0f;
    int selectedOperation_ = 0;
};

/**
 * @brief Lektion 4: Tensor-Datenbanken
 */
class Lesson4_Database : public Lesson {
public:
    Lesson4_Database();
    void init() override;
};

} // namespace lessons
