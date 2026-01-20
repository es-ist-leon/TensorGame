#pragma once

#include "raylib.h"
#include "gui/Colors.hpp"
#include <string>
#include <vector>
#include <functional>

namespace gui {

/**
 * @brief UI-Komponenten für das Lernspiel
 */
class UIComponents {
public:
    // Initialisierung
    static void init(int screenWidth, int screenHeight);

    // Button
    static bool button(Rectangle bounds, const std::string& text, bool enabled = true);
    static bool iconButton(Rectangle bounds, const std::string& icon, const std::string& tooltip = "");

    // Panel
    static void panel(Rectangle bounds, const std::string& title = "");
    static void panelWithBorder(Rectangle bounds, const std::string& title, Color borderColor);

    // Text
    static void label(Vector2 pos, const std::string& text, int fontSize = 20, Color color = Colors::TEXT);
    static void labelCentered(Rectangle bounds, const std::string& text, int fontSize = 20, Color color = Colors::TEXT);
    static void title(Vector2 pos, const std::string& text);
    static void subtitle(Vector2 pos, const std::string& text);
    static void paragraph(Rectangle bounds, const std::string& text, int fontSize = 18);

    // Code-Anzeige
    static void codeBlock(Rectangle bounds, const std::string& code, const std::string& language = "cpp");
    static void codeBlockAnimated(Rectangle bounds, const std::string& code, float progress);

    // Fortschrittsanzeige
    static void progressBar(Rectangle bounds, float progress, const std::string& label = "");
    static void stepIndicator(Rectangle bounds, int currentStep, int totalSteps, const std::vector<std::string>& labels);

    // Listen
    static int listBox(Rectangle bounds, const std::vector<std::string>& items, int selectedIndex);
    static void infoCard(Rectangle bounds, const std::string& title, const std::string& content, Color accentColor);

    // Eingabe
    static bool textInput(Rectangle bounds, std::string& text, const std::string& placeholder = "");
    static float slider(Rectangle bounds, float value, float min, float max, const std::string& label = "");

    // Tooltips
    static void tooltip(const std::string& text);
    static void showTooltipAt(Vector2 pos, const std::string& text);

    // Nachrichten
    static void notification(const std::string& message, Color color = Colors::PRIMARY);
    static void successMessage(const std::string& message);
    static void errorMessage(const std::string& message);

    // Navigation
    static int tabBar(Rectangle bounds, const std::vector<std::string>& tabs, int selectedTab);
    static bool backButton(Vector2 pos);
    static bool nextButton(Vector2 pos, const std::string& text = "Weiter");

    // Animations-Helpers
    static float easeInOut(float t);
    static float easeOut(float t);
    static Color lerpColor(Color a, Color b, float t);

    // Frame-unabhängige Animation
    static void updateAnimations(float deltaTime);

private:
    static int screenWidth_;
    static int screenHeight_;
    static std::string currentTooltip_;
    static Vector2 tooltipPos_;
    static float tooltipAlpha_;
    static std::vector<std::pair<std::string, float>> notifications_;
};

/**
 * @brief Animierter Text-Schreiber (Typewriter-Effekt)
 */
class TypeWriter {
public:
    TypeWriter() = default;
    TypeWriter(const std::string& text, float charsPerSecond = 30.0f);

    void start(const std::string& text);
    void update(float deltaTime);
    void skip();
    bool isComplete() const;
    std::string getCurrentText() const;
    float getProgress() const;

private:
    std::string fullText_;
    float charsPerSecond_ = 30.0f;
    float currentChar_ = 0.0f;
    bool complete_ = true;
};

/**
 * @brief Scroll-Container für lange Inhalte
 */
class ScrollPanel {
public:
    ScrollPanel(Rectangle bounds);

    void begin();
    void end();
    void setContentHeight(float height);
    float getScrollOffset() const { return scrollOffset_; }

private:
    Rectangle bounds_;
    float contentHeight_ = 0;
    float scrollOffset_ = 0;
    float scrollVelocity_ = 0;
};

} // namespace gui
