#include "gui/Application.hpp"
#include <cmath>

namespace gui {

Application* Application::instance_ = nullptr;

Application::Application() {
    instance_ = this;
}

Application::~Application() {
    shutdown();
    instance_ = nullptr;
}

Application& Application::getInstance() {
    return *instance_;
}

void Application::run() {
    init();

    while (!WindowShouldClose()) {
        update();
        render();
    }

    shutdown();
}

void Application::init() {
    // Fenster initialisieren
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth_, screenHeight_, "Tensor Learning Game - Lerne Tensoren verstehen!");
    SetTargetFPS(60);

    // UI initialisieren
    UIComponents::init(screenWidth_, screenHeight_);

    // Module erstellen
    lessonManager_ = std::make_unique<lessons::LessonManager>();
    sandbox_ = std::make_unique<sandbox::Sandbox>();

    // Demo-Tensor für Hauptmenü
    demoTensor_ = tensor::Tensor::random({4, 4, 4}, 0.0f, 1.0f);
    mainVisualizer_.setTensor(demoTensor_, "Demo Tensor");
    mainVisualizer_.setMode(VisualizationMode::Cubes);
    mainVisualizer_.setShowGrid(false);
    mainVisualizer_.setShowAxes(false);

    // Beispiel-Tensoren in DB laden
    database_.store("beispiel_vektor", tensor::Tensor::range(1, 6), "Ein einfacher Vektor [1,2,3,4,5]");
    database_.store("beispiel_matrix", tensor::Tensor::identity(3), "3x3 Einheitsmatrix");
    database_.store("zufalls_tensor", tensor::Tensor::random({2, 3, 4}), "Zufälliger 3D-Tensor");
}

void Application::shutdown() {
    lessonManager_.reset();
    sandbox_.reset();
    CloseWindow();
}

void Application::update() {
    float deltaTime = GetFrameTime();

    // Fenstergröße aktualisieren
    if (IsWindowResized()) {
        screenWidth_ = GetScreenWidth();
        screenHeight_ = GetScreenHeight();
        UIComponents::init(screenWidth_, screenHeight_);
    }

    // Fullscreen Toggle
    if (IsKeyPressed(KEY_F11)) {
        fullscreen_ = !fullscreen_;
        ToggleFullscreen();
    }

    // ESC für Zurück zum Menü
    if (IsKeyPressed(KEY_ESCAPE) && currentState_ != AppState::MainMenu) {
        setState(AppState::MainMenu);
    }

    // Zustandsübergang
    if (stateTransition_ < 1.0f) {
        stateTransition_ += deltaTime * 3.0f;
        if (stateTransition_ > 1.0f) stateTransition_ = 1.0f;
    }

    // Menü-Animation
    menuAnimationTime_ += deltaTime;
    demoRotation_ += deltaTime * 20.0f;

    // Zustandsspezifisches Update
    switch (currentState_) {
        case AppState::MainMenu: updateMainMenu(); break;
        case AppState::Lessons: updateLessons(); break;
        case AppState::Sandbox: updateSandbox(); break;
        case AppState::Database: updateDatabase(); break;
        case AppState::Settings: updateSettings(); break;
    }

    UIComponents::updateAnimations(deltaTime);
}

void Application::render() {
    BeginDrawing();
    ClearBackground(Colors::BACKGROUND);

    // Zustandsspezifisches Rendering
    switch (currentState_) {
        case AppState::MainMenu: renderMainMenu(); break;
        case AppState::Lessons: renderLessons(); break;
        case AppState::Sandbox: renderSandbox(); break;
        case AppState::Database: renderDatabase(); break;
        case AppState::Settings: renderSettings(); break;
    }

    // FPS anzeigen
    if (showFPS_) {
        DrawText(TextFormat("FPS: %d", GetFPS()), screenWidth_ - 80, 10, 16, Colors::TEXT_DIM);
    }

    EndDrawing();
}

void Application::setState(AppState state) {
    previousState_ = currentState_;
    currentState_ = state;
    stateTransition_ = 0.0f;
}

void Application::updateMainMenu() {
    mainVisualizer_.setCameraAngle(demoRotation_, 25.0f);
    mainVisualizer_.update(GetFrameTime());
}

void Application::renderMainMenu() {
    // Hintergrund-Tensor-Visualisierung
    mainVisualizer_.render3D();

    // Semi-transparentes Overlay
    DrawRectangle(0, 0, screenWidth_, screenHeight_, {15, 15, 25, 200});

    // Titel
    const char* title = "TENSOR LEARNING GAME";
    int titleSize = 48;
    int titleWidth = MeasureText(title, titleSize);
    float titleY = 80;

    // Titel mit Glow-Effekt
    for (int i = 3; i > 0; --i) {
        DrawText(title, (screenWidth_ - titleWidth) / 2, static_cast<int>(titleY),
                 titleSize, {Colors::PRIMARY.r, Colors::PRIMARY.g, Colors::PRIMARY.b, static_cast<unsigned char>(50 / i)});
    }
    DrawText(title, (screenWidth_ - titleWidth) / 2, static_cast<int>(titleY), titleSize, Colors::TEXT_HIGHLIGHT);

    // Untertitel
    const char* subtitle = "Verstehe Tensoren durch interaktive 3D-Visualisierung";
    int subtitleWidth = MeasureText(subtitle, 22);
    DrawText(subtitle, (screenWidth_ - subtitleWidth) / 2, static_cast<int>(titleY + 60), 22, Colors::TEXT_DIM);

    // Menü-Buttons
    float buttonWidth = 300;
    float buttonHeight = 60;
    float buttonSpacing = 20;
    float menuX = (screenWidth_ - buttonWidth) / 2;
    float menuY = screenHeight_ / 2 - 50;

    struct MenuItem {
        const char* text;
        const char* description;
        AppState state;
        Color color;
    };

    MenuItem items[] = {
        {"Lektionen", "Lerne Tensoren Schritt fuer Schritt", AppState::Lessons, Colors::PRIMARY},
        {"Sandbox", "Experimentiere frei mit Tensoren", AppState::Sandbox, Colors::ACCENT_GREEN},
        {"Datenbank", "Verwalte und erkunde Tensoren", AppState::Database, Colors::ACCENT_PURPLE},
        {"Einstellungen", "Passe das Spiel an", AppState::Settings, Colors::TEXT_DIM}
    };

    for (int i = 0; i < 4; ++i) {
        float y = menuY + i * (buttonHeight + buttonSpacing);
        Rectangle bounds = {menuX, y, buttonWidth, buttonHeight};

        // Animation
        float anim = sinf(menuAnimationTime_ * 2.0f + i * 0.5f) * 0.5f + 0.5f;
        bounds.x -= anim * 5;

        // Button mit Farbakzent
        Vector2 mouse = GetMousePosition();
        bool hover = CheckCollisionPointRec(mouse, bounds);

        Color bgColor = hover ? Colors::BUTTON_HOVER : Colors::BUTTON;
        DrawRectangleRounded(bounds, 0.2f, 8, bgColor);
        DrawRectangleRoundedLines(bounds, 0.2f, 8, 2, items[i].color);

        // Icon-Bereich
        DrawRectangleRounded({bounds.x, bounds.y, 8, bounds.height}, 0.5f, 4, items[i].color);

        // Text
        DrawText(items[i].text, static_cast<int>(bounds.x + 25), static_cast<int>(bounds.y + 12), 24, Colors::TEXT_HIGHLIGHT);
        DrawText(items[i].description, static_cast<int>(bounds.x + 25), static_cast<int>(bounds.y + 38), 14, Colors::TEXT_DIM);

        if (hover && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            setState(items[i].state);
        }
    }

    // Footer
    const char* footer = "Druecke F11 fuer Vollbild | ESC fuer Zurueck | Entwickelt mit Raylib & C++";
    int footerWidth = MeasureText(footer, 14);
    DrawText(footer, (screenWidth_ - footerWidth) / 2, screenHeight_ - 40, 14, Colors::TEXT_DIM);

    // Version
    DrawText("v1.0.0", 10, screenHeight_ - 30, 14, Colors::TEXT_DIM);
}

void Application::updateLessons() {
    lessonManager_->update(GetFrameTime());
}

void Application::renderLessons() {
    renderHeader();
    lessonManager_->render();
    renderFooter();
}

void Application::updateSandbox() {
    sandbox_->update(GetFrameTime());
}

void Application::renderSandbox() {
    renderHeader();
    sandbox_->render();
    renderFooter();
}

void Application::updateDatabase() {
    // Database View Update
}

void Application::renderDatabase() {
    renderHeader();

    // Linkes Panel: Liste
    float listWidth = 300;
    Rectangle listBounds = {20, 80, listWidth, static_cast<float>(screenHeight_) - 160};
    UIComponents::panel(listBounds, "Gespeicherte Tensoren");

    auto names = database_.listNames();
    static int selectedIndex = -1;

    float itemY = listBounds.y + 50;
    for (size_t i = 0; i < names.size(); ++i) {
        Rectangle itemBounds = {listBounds.x + 10, itemY, listBounds.width - 20, 50};

        auto meta = database_.getMetadata(names[i]);
        bool hover = CheckCollisionPointRec(GetMousePosition(), itemBounds);
        bool selected = (static_cast<int>(i) == selectedIndex);

        if (hover || selected) {
            DrawRectangleRounded(itemBounds, 0.2f, 4, selected ? Colors::PRIMARY_DARK : Colors::BUTTON_HOVER);
        }

        if (hover && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            selectedIndex = static_cast<int>(i);
            auto tensor = database_.get(names[i]);
            if (tensor) {
                mainVisualizer_.setTensor(*tensor, names[i]);
            }
        }

        DrawText(names[i].c_str(), static_cast<int>(itemBounds.x + 10), static_cast<int>(itemBounds.y + 8), 18, Colors::TEXT);
        if (meta) {
            DrawText(meta->shapeString().c_str(), static_cast<int>(itemBounds.x + 10),
                     static_cast<int>(itemBounds.y + 28), 14, Colors::TEXT_DIM);
        }

        itemY += 55;
    }

    // DB Stats
    auto stats = database_.getStats();
    DrawText(TextFormat("Tensoren: %zu", stats.tensorCount),
             static_cast<int>(listBounds.x + 10), static_cast<int>(listBounds.y + listBounds.height - 60), 14, Colors::TEXT_DIM);
    DrawText(TextFormat("Elemente: %zu", stats.totalElements),
             static_cast<int>(listBounds.x + 10), static_cast<int>(listBounds.y + listBounds.height - 40), 14, Colors::TEXT_DIM);

    // Rechtes Panel: Visualisierung
    float vizX = listWidth + 40;
    float vizWidth = screenWidth_ - vizX - 20;
    Rectangle vizBounds = {vizX, 80, vizWidth, static_cast<float>(screenHeight_) - 160};

    DrawRectangleRounded(vizBounds, 0.01f, 4, Colors::BACKGROUND_PANEL);
    DrawRectangleRoundedLines(vizBounds, 0.01f, 4, 1, Colors::BORDER);

    // 3D-Ansicht
    BeginScissorMode(static_cast<int>(vizBounds.x), static_cast<int>(vizBounds.y),
                     static_cast<int>(vizBounds.width), static_cast<int>(vizBounds.height));

    mainVisualizer_.update(GetFrameTime());
    mainVisualizer_.render3D();

    EndScissorMode();

    mainVisualizer_.renderOverlay();

    // Info-Panel wenn ausgewählt
    if (selectedIndex >= 0 && selectedIndex < static_cast<int>(names.size())) {
        auto meta = database_.getMetadata(names[selectedIndex]);
        if (meta) {
            float infoY = vizBounds.y + vizBounds.height - 120;
            DrawRectangle(static_cast<int>(vizBounds.x), static_cast<int>(infoY),
                          static_cast<int>(vizBounds.width), 120, {30, 30, 50, 230});

            DrawText(meta->name.c_str(), static_cast<int>(vizBounds.x + 15), static_cast<int>(infoY + 10), 20, Colors::TEXT_HIGHLIGHT);
            DrawText(meta->description.c_str(), static_cast<int>(vizBounds.x + 15), static_cast<int>(infoY + 35), 16, Colors::TEXT);
            DrawText(TextFormat("Shape: %s | Rank: %zu | Elemente: %zu",
                                meta->shapeString().c_str(), meta->shape.size(), meta->size),
                     static_cast<int>(vizBounds.x + 15), static_cast<int>(infoY + 60), 14, Colors::TEXT_DIM);
            DrawText(TextFormat("Erstellt: %s", meta->createdString().c_str()),
                     static_cast<int>(vizBounds.x + 15), static_cast<int>(infoY + 80), 14, Colors::TEXT_DIM);
        }
    }

    renderFooter();
}

void Application::updateSettings() {
    // Settings Update
}

void Application::renderSettings() {
    renderHeader();

    float panelWidth = 600;
    float panelX = (screenWidth_ - panelWidth) / 2;
    Rectangle bounds = {panelX, 100, panelWidth, static_cast<float>(screenHeight_) - 200};

    UIComponents::panel(bounds, "Einstellungen");

    float y = bounds.y + 60;
    float labelX = bounds.x + 30;
    float controlX = bounds.x + 250;
    float controlWidth = 300;

    // Qualität
    DrawText("Visualisierungsqualitaet:", static_cast<int>(labelX), static_cast<int>(y), 18, Colors::TEXT);
    const char* qualityLabels[] = {"Niedrig", "Mittel", "Hoch"};
    for (int i = 0; i < 3; ++i) {
        Rectangle btnBounds = {controlX + i * 100, y - 5, 90, 35};
        bool selected = (visualizationQuality_ == i);

        Color bgColor = selected ? Colors::PRIMARY : Colors::BUTTON;
        DrawRectangleRounded(btnBounds, 0.3f, 4, bgColor);

        if (CheckCollisionPointRec(GetMousePosition(), btnBounds) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            visualizationQuality_ = i;
        }

        UIComponents::labelCentered(btnBounds, qualityLabels[i], 16, Colors::TEXT);
    }
    y += 60;

    // FPS anzeigen
    DrawText("FPS anzeigen:", static_cast<int>(labelX), static_cast<int>(y), 18, Colors::TEXT);
    Rectangle fpsToggle = {controlX, y - 5, 80, 35};
    DrawRectangleRounded(fpsToggle, 0.5f, 4, showFPS_ ? Colors::ACCENT_GREEN : Colors::BUTTON);
    UIComponents::labelCentered(fpsToggle, showFPS_ ? "AN" : "AUS", 16, Colors::TEXT);
    if (CheckCollisionPointRec(GetMousePosition(), fpsToggle) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        showFPS_ = !showFPS_;
    }
    y += 60;

    // Vollbild
    DrawText("Vollbild:", static_cast<int>(labelX), static_cast<int>(y), 18, Colors::TEXT);
    Rectangle fullToggle = {controlX, y - 5, 80, 35};
    DrawRectangleRounded(fullToggle, 0.5f, 4, fullscreen_ ? Colors::ACCENT_GREEN : Colors::BUTTON);
    UIComponents::labelCentered(fullToggle, fullscreen_ ? "AN" : "AUS", 16, Colors::TEXT);
    if (CheckCollisionPointRec(GetMousePosition(), fullToggle) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        fullscreen_ = !fullscreen_;
        ToggleFullscreen();
    }
    y += 80;

    // Tastenkürzel
    DrawText("Tastenkuerzel:", static_cast<int>(labelX), static_cast<int>(y), 20, Colors::PRIMARY_LIGHT);
    y += 35;

    const char* shortcuts[] = {
        "F11 - Vollbild umschalten",
        "ESC - Zurueck zum Menue",
        "R - Kamera zuruecksetzen",
        "G - Grid ein/aus",
        "X - Achsen ein/aus",
        "V - Werte ein/aus",
        "1-7 - Visualisierungsmodus",
        "WASD - Kamera bewegen",
        "Rechte Maus - Kamera drehen",
        "Mausrad - Zoom"
    };

    for (const char* shortcut : shortcuts) {
        DrawText(shortcut, static_cast<int>(labelX + 20), static_cast<int>(y), 16, Colors::TEXT_DIM);
        y += 25;
    }

    renderFooter();
}

void Application::renderHeader() {
    // Header-Leiste
    DrawRectangle(0, 0, screenWidth_, 60, Colors::BACKGROUND_LIGHT);
    DrawLine(0, 60, screenWidth_, 60, Colors::BORDER);

    // Zurück-Button
    if (UIComponents::button({10, 10, 100, 40}, "< Menue")) {
        setState(AppState::MainMenu);
    }

    // Titel je nach Zustand
    const char* title = "";
    switch (currentState_) {
        case AppState::Lessons: title = "Lektionen"; break;
        case AppState::Sandbox: title = "Sandbox"; break;
        case AppState::Database: title = "Tensor-Datenbank"; break;
        case AppState::Settings: title = "Einstellungen"; break;
        default: break;
    }

    int titleWidth = MeasureText(title, 28);
    DrawText(title, (screenWidth_ - titleWidth) / 2, 15, 28, Colors::TEXT_HIGHLIGHT);
}

void Application::renderFooter() {
    // Footer nur minimal
    DrawText("ESC - Menue | F11 - Vollbild", 10, screenHeight_ - 25, 12, Colors::TEXT_DIM);
}

} // namespace gui
