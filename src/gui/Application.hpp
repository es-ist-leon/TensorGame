#pragma once

#include "raylib.h"
#include "gui/UIComponents.hpp"
#include "gui/TensorVisualizer.hpp"
#include "tensor/Tensor.hpp"
#include "tensor/TensorDB.hpp"
#include "lessons/LessonManager.hpp"
#include "sandbox/Sandbox.hpp"
#include <memory>
#include <string>

namespace gui {

/**
 * @brief Anwendungszustände
 */
enum class AppState {
    MainMenu,
    Lessons,
    Sandbox,
    Database,
    Settings
};

/**
 * @brief Hauptanwendungsklasse
 */
class Application {
public:
    Application();
    ~Application();

    // Anwendung ausführen
    void run();

    // Singleton-Zugriff
    static Application& getInstance();

private:
    // Initialisierung
    void init();
    void shutdown();

    // Haupt-Loop
    void update();
    void render();

    // Zustandsmanagement
    void setState(AppState state);
    void updateMainMenu();
    void updateLessons();
    void updateSandbox();
    void updateDatabase();
    void updateSettings();

    // Rendering
    void renderMainMenu();
    void renderLessons();
    void renderSandbox();
    void renderDatabase();
    void renderSettings();

    // UI-Hilfsfunktionen
    void renderHeader();
    void renderFooter();

    // Fenster-Einstellungen
    int screenWidth_ = 1600;
    int screenHeight_ = 900;
    bool fullscreen_ = false;

    // Zustand
    AppState currentState_ = AppState::MainMenu;
    AppState previousState_ = AppState::MainMenu;
    float stateTransition_ = 1.0f;

    // Module
    std::unique_ptr<lessons::LessonManager> lessonManager_;
    std::unique_ptr<sandbox::Sandbox> sandbox_;
    tensor::TensorDB database_;

    // Visuals
    TensorVisualizer mainVisualizer_;
    float menuAnimationTime_ = 0.0f;

    // Settings
    float masterVolume_ = 0.8f;
    bool showFPS_ = true;
    int visualizationQuality_ = 2;  // 0=Low, 1=Medium, 2=High

    // Demo-Tensoren für Hauptmenü
    tensor::Tensor demoTensor_;
    float demoRotation_ = 0.0f;

    static Application* instance_;
};

} // namespace gui
