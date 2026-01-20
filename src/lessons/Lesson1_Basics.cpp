#include "lessons/Lesson.hpp"

namespace lessons {

// === Basis Lesson Implementierung ===

Lesson::Lesson(const std::string& title, const std::string& description)
    : title_(title), description_(description) {}

void Lesson::init() {
    currentStep_ = 0;
    codeProgress_ = 0.0f;

    if (!steps_.empty()) {
        if (steps_[0].tensorGenerator) {
            visualizer_.setTensor(steps_[0].tensorGenerator(), steps_[0].title);
        }
        visualizer_.setMode(steps_[0].visualMode);
        typeWriter_.start(steps_[0].content);
    }

    // Layout berechnen
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    contentBounds_ = {20, 80, static_cast<float>(screenWidth) * 0.35f, static_cast<float>(screenHeight) - 180};
    vizBounds_ = {contentBounds_.x + contentBounds_.width + 20, 80,
                  static_cast<float>(screenWidth) - contentBounds_.width - 60,
                  static_cast<float>(screenHeight) - 180};
    codeBounds_ = {contentBounds_.x, contentBounds_.y + contentBounds_.height * 0.6f,
                   contentBounds_.width, contentBounds_.height * 0.38f};
}

void Lesson::update(float deltaTime) {
    typeWriter_.update(deltaTime);

    // Code-Animation
    if (showCode_ && !steps_.empty() && !steps_[currentStep_].code.empty()) {
        if (typeWriter_.isComplete() && codeProgress_ < 1.0f) {
            codeProgress_ += deltaTime * 0.5f;
            if (codeProgress_ > 1.0f) codeProgress_ = 1.0f;
        }
    }

    visualizer_.update(deltaTime);

    // Skip mit Leertaste
    if (IsKeyPressed(KEY_SPACE)) {
        if (!typeWriter_.isComplete()) {
            typeWriter_.skip();
        } else if (codeProgress_ < 1.0f) {
            codeProgress_ = 1.0f;
        }
    }

    // Navigation mit Pfeiltasten
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_ENTER)) {
        nextStep();
    }
    if (IsKeyPressed(KEY_LEFT)) {
        previousStep();
    }
}

void Lesson::render() {
    renderContent();
    renderVisualization();
    renderNavigation();
}

void Lesson::cleanup() {
    // Aufräumen falls nötig
}

void Lesson::nextStep() {
    if (currentStep_ < static_cast<int>(steps_.size()) - 1) {
        currentStep_++;
        codeProgress_ = 0.0f;

        auto& step = steps_[currentStep_];
        typeWriter_.start(step.content);

        if (step.tensorGenerator) {
            visualizer_.setTensor(step.tensorGenerator(), step.title);
        }
        visualizer_.setMode(step.visualMode);
    }
}

void Lesson::previousStep() {
    if (currentStep_ > 0) {
        currentStep_--;
        codeProgress_ = 1.0f;  // Code sofort zeigen bei Zurück

        auto& step = steps_[currentStep_];
        typeWriter_.start(step.content);
        typeWriter_.skip();

        if (step.tensorGenerator) {
            visualizer_.setTensor(step.tensorGenerator(), step.title);
        }
        visualizer_.setMode(step.visualMode);
    }
}

bool Lesson::isComplete() const {
    return currentStep_ >= static_cast<int>(steps_.size()) - 1 && typeWriter_.isComplete();
}

void Lesson::addStep(const LessonStep& step) {
    steps_.push_back(step);
}

void Lesson::renderContent() {
    if (steps_.empty()) return;

    auto& step = steps_[currentStep_];

    // Content Panel
    gui::UIComponents::panel(contentBounds_, step.title);

    // Text
    Rectangle textBounds = {contentBounds_.x + 15, contentBounds_.y + 55,
                            contentBounds_.width - 30, contentBounds_.height * 0.5f - 70};
    gui::UIComponents::paragraph(textBounds, typeWriter_.getCurrentText(), 18);

    // Code-Block wenn vorhanden
    if (!step.code.empty() && showCode_) {
        Rectangle actualCodeBounds = {contentBounds_.x + 10, contentBounds_.y + contentBounds_.height * 0.5f,
                                       contentBounds_.width - 20, contentBounds_.height * 0.48f};
        gui::UIComponents::codeBlockAnimated(actualCodeBounds, step.code, codeProgress_);
    }

    // Interaktiver Hinweis
    if (step.interactive && !step.interactiveHint.empty()) {
        DrawText(step.interactiveHint.c_str(),
                 static_cast<int>(contentBounds_.x + 15),
                 static_cast<int>(contentBounds_.y + contentBounds_.height - 30),
                 14, gui::Colors::ACCENT_YELLOW);
    }
}

void Lesson::renderVisualization() {
    // Viz Panel
    DrawRectangleRounded(vizBounds_, 0.01f, 4, gui::Colors::BACKGROUND_PANEL);
    DrawRectangleRoundedLines(vizBounds_, 0.01f, 4, 1, gui::Colors::BORDER);

    // 3D Ansicht
    BeginScissorMode(static_cast<int>(vizBounds_.x), static_cast<int>(vizBounds_.y),
                     static_cast<int>(vizBounds_.width), static_cast<int>(vizBounds_.height));

    visualizer_.render3D();

    EndScissorMode();

    visualizer_.renderOverlay();
}

void Lesson::renderNavigation() {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    // Schritt-Anzeige
    std::vector<std::string> stepLabels;
    for (size_t i = 0; i < steps_.size(); ++i) {
        stepLabels.push_back(std::to_string(i + 1));
    }

    gui::UIComponents::stepIndicator({static_cast<float>(screenWidth) / 2 - 200, static_cast<float>(screenHeight) - 70, 400, 50},
                                     currentStep_, static_cast<int>(steps_.size()), stepLabels);

    // Navigationsbuttons
    if (currentStep_ > 0) {
        if (gui::UIComponents::button({20, static_cast<float>(screenHeight) - 70, 120, 40}, "< Zurueck")) {
            previousStep();
        }
    }

    if (currentStep_ < static_cast<int>(steps_.size()) - 1) {
        if (gui::UIComponents::button({static_cast<float>(screenWidth) - 140, static_cast<float>(screenHeight) - 70, 120, 40}, "Weiter >")) {
            nextStep();
        }
    } else {
        // Fertig-Button
        DrawRectangleRounded({static_cast<float>(screenWidth) - 140, static_cast<float>(screenHeight) - 70, 120, 40}, 0.3f, 4, gui::Colors::ACCENT_GREEN);
        gui::UIComponents::labelCentered({static_cast<float>(screenWidth) - 140, static_cast<float>(screenHeight) - 70, 120, 40}, "Fertig!", 18, gui::Colors::TEXT_HIGHLIGHT);
    }

    // Hinweis
    DrawText("Leertaste: Ueberspringen | Pfeiltasten: Navigation", 20, screenHeight - 25, 12, gui::Colors::TEXT_DIM);
}

// === Lektion 1: Tensor-Grundlagen ===

Lesson1_Basics::Lesson1_Basics()
    : Lesson("Tensor Grundlagen", "Verstehe was Tensoren sind und wie sie strukturiert sind") {}

void Lesson1_Basics::init() {
    // Schritt 1: Einführung
    addStep({
        "Was ist ein Tensor?",
        "Ein Tensor ist eine mathematische Verallgemeinerung von Skalaren, Vektoren und Matrizen auf beliebig viele Dimensionen.\n\n"
        "Tensoren sind das fundamentale Datenformat in Machine Learning, Physik und vielen anderen Bereichen.\n\n"
        "Der 'Rang' eines Tensors gibt an, wie viele Dimensionen er hat.",
        "// Tensoren nach Rang:\n"
        "// Rang 0: Skalar    (einzelne Zahl)\n"
        "// Rang 1: Vektor    (1D Array)\n"
        "// Rang 2: Matrix    (2D Array)\n"
        "// Rang 3+: Tensor   (nD Array)",
        []() { return tensor::Tensor(42.0f); },  // Skalar
        gui::VisualizationMode::Cubes
    });

    // Schritt 2: Skalar
    addStep({
        "Rang 0: Skalar",
        "Ein Skalar ist ein einzelner Wert - die einfachste Form eines Tensors.\n\n"
        "In der 3D-Ansicht siehst du einen einzelnen Würfel, der diesen Wert repräsentiert.\n\n"
        "Shape: () - leere Klammern bedeuten keine Dimensionen.",
        "// C++ Code:\n"
        "Tensor skalar(42.0f);\n"
        "\n"
        "// Eigenschaften:\n"
        "skalar.rank();  // = 0\n"
        "skalar.size();  // = 1\n"
        "skalar.shape(); // = ()",
        []() { return tensor::Tensor(42.0f); },
        gui::VisualizationMode::Cubes
    });

    // Schritt 3: Vektor
    addStep({
        "Rang 1: Vektor",
        "Ein Vektor ist eine eindimensionale Liste von Werten.\n\n"
        "In der Visualisierung siehst du die Werte als Reihe von Würfeln. Die Farbe zeigt den Wert - von blau (niedrig) über grün zu rot (hoch).\n\n"
        "Shape: (5) bedeutet 5 Elemente in einer Dimension.",
        "// C++ Code:\n"
        "Tensor vektor({5});  // 5 Elemente\n"
        "vektor[0] = 1.0f;\n"
        "vektor[1] = 2.0f;\n"
        "// ...\n"
        "\n"
        "// Oder direkt:\n"
        "auto v = Tensor::range(1, 6);",
        []() { return tensor::Tensor::range(1, 6); },
        gui::VisualizationMode::Bars
    });

    // Schritt 4: Matrix
    addStep({
        "Rang 2: Matrix",
        "Eine Matrix ist ein zweidimensionales Array - Zeilen und Spalten.\n\n"
        "Hier siehst du eine 3x3 Einheitsmatrix. Die Diagonale hat Werte von 1 (gelb), der Rest ist 0 (blau).\n\n"
        "Shape: (3, 3) bedeutet 3 Zeilen und 3 Spalten.",
        "// C++ Code:\n"
        "Tensor matrix({3, 3});\n"
        "\n"
        "// Einheitsmatrix erstellen:\n"
        "auto I = Tensor::identity(3);\n"
        "\n"
        "// Zugriff:\n"
        "matrix.at(0, 0) = 1.0f;  // Zeile 0, Spalte 0",
        []() { return tensor::Tensor::identity(3); },
        gui::VisualizationMode::Heatmap
    });

    // Schritt 5: 3D Tensor
    addStep({
        "Rang 3: 3D Tensor",
        "Ein 3D-Tensor hat drei Dimensionen. Stell dir einen Würfel aus Zahlen vor!\n\n"
        "Dies ist besonders nützlich für Bilddaten (Höhe x Breite x Farbkanäle) oder Zeitreihen.\n\n"
        "Shape: (3, 3, 3) bedeutet ein 3x3x3 Würfel mit 27 Elementen.",
        "// C++ Code:\n"
        "Tensor tensor3d({3, 3, 3});\n"
        "\n"
        "// Zufällige Werte:\n"
        "auto t = Tensor::random({3, 3, 3});\n"
        "\n"
        "// Zugriff:\n"
        "tensor3d.at(0, 1, 2) = 5.0f;",
        []() { return tensor::Tensor::random({3, 3, 3}); },
        gui::VisualizationMode::Cubes
    });

    // Schritt 6: Shape und Strides
    addStep({
        "Shape und Strides",
        "Shape beschreibt die Größe jeder Dimension.\n\n"
        "Strides beschreiben, wie viele Elemente übersprungen werden müssen, um zur nächsten Position in einer Dimension zu gelangen.\n\n"
        "Dies ermöglicht effiziente Speicherung als 1D-Array!",
        "// Shape: (2, 3, 4) bedeutet:\n"
        "// - 2 'Schichten'\n"
        "// - 3 Zeilen pro Schicht\n"
        "// - 4 Spalten pro Zeile\n"
        "\n"
        "// Strides: (12, 4, 1)\n"
        "// - +12 fuer naechste Schicht\n"
        "// - +4 fuer naechste Zeile\n"
        "// - +1 fuer naechste Spalte",
        []() { return tensor::Tensor::random({2, 3, 4}); },
        gui::VisualizationMode::Memory
    });

    // Schritt 7: Zusammenfassung
    addStep({
        "Zusammenfassung",
        "Du hast gelernt:\n\n"
        "• Tensoren sind n-dimensionale Arrays\n"
        "• Rang = Anzahl der Dimensionen\n"
        "• Shape = Größe jeder Dimension\n"
        "• Skalare (0D), Vektoren (1D), Matrizen (2D), und höher\n\n"
        "In der nächsten Lektion lernst du, wie du Tensoren in C++ erstellst!",
        "// Tensor-Hierarchie:\n"
        "//\n"
        "// Skalar  < Vektor < Matrix < Tensor\n"
        "// (0D)      (1D)     (2D)     (nD)\n"
        "//\n"
        "// Alle sind Tensoren!\n"
        "// Der Rang ist der einzige Unterschied.",
        []() { return tensor::Tensor::random({4, 4, 4}); },
        gui::VisualizationMode::Network
    });

    Lesson::init();
}

} // namespace lessons
