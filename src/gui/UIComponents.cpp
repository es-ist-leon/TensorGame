#include "gui/UIComponents.hpp"
#include <algorithm>
#include <cmath>

namespace gui {

// Static members
int UIComponents::screenWidth_ = 1280;
int UIComponents::screenHeight_ = 720;
std::string UIComponents::currentTooltip_ = "";
Vector2 UIComponents::tooltipPos_ = {0, 0};
float UIComponents::tooltipAlpha_ = 0;
std::vector<std::pair<std::string, float>> UIComponents::notifications_;

void UIComponents::init(int screenWidth, int screenHeight) {
    screenWidth_ = screenWidth;
    screenHeight_ = screenHeight;
}

bool UIComponents::button(Rectangle bounds, const std::string& text, bool enabled) {
    bool clicked = false;
    Color bgColor = Colors::BUTTON;
    Color textColor = Colors::TEXT;

    if (!enabled) {
        bgColor = {40, 40, 60, 255};
        textColor = Colors::TEXT_DIM;
    } else {
        Vector2 mouse = GetMousePosition();
        if (CheckCollisionPointRec(mouse, bounds)) {
            bgColor = Colors::BUTTON_HOVER;
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                bgColor = Colors::BUTTON_ACTIVE;
            }
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                clicked = true;
            }
        }
    }

    // Zeichne Button mit abgerundeten Ecken (simuliert)
    DrawRectangleRounded(bounds, 0.3f, 8, bgColor);
    DrawRectangleRoundedLines(bounds, 0.3f, 8, 2, Colors::BORDER);

    // Text zentrieren
    int fontSize = 18;
    int textWidth = MeasureText(text.c_str(), fontSize);
    float textX = bounds.x + (bounds.width - textWidth) / 2;
    float textY = bounds.y + (bounds.height - fontSize) / 2;
    DrawText(text.c_str(), static_cast<int>(textX), static_cast<int>(textY), fontSize, textColor);

    return clicked;
}

bool UIComponents::iconButton(Rectangle bounds, const std::string& icon, const std::string& tooltip) {
    bool clicked = button(bounds, icon, true);

    if (!tooltip.empty()) {
        Vector2 mouse = GetMousePosition();
        if (CheckCollisionPointRec(mouse, bounds)) {
            currentTooltip_ = tooltip;
            tooltipPos_ = {mouse.x + 10, mouse.y + 10};
        }
    }

    return clicked;
}

void UIComponents::panel(Rectangle bounds, const std::string& title) {
    DrawRectangleRounded(bounds, 0.02f, 4, Colors::BACKGROUND_PANEL);
    DrawRectangleRoundedLines(bounds, 0.02f, 4, 1, Colors::BORDER);

    if (!title.empty()) {
        DrawRectangleRounded({bounds.x, bounds.y, bounds.width, 40}, 0.1f, 4, Colors::BACKGROUND_LIGHT);
        DrawText(title.c_str(), static_cast<int>(bounds.x + 15), static_cast<int>(bounds.y + 10), 20, Colors::TEXT);
        DrawLine(static_cast<int>(bounds.x), static_cast<int>(bounds.y + 40),
                 static_cast<int>(bounds.x + bounds.width), static_cast<int>(bounds.y + 40), Colors::BORDER);
    }
}

void UIComponents::panelWithBorder(Rectangle bounds, const std::string& title, Color borderColor) {
    DrawRectangleRounded(bounds, 0.02f, 4, Colors::BACKGROUND_PANEL);
    DrawRectangleRoundedLines(bounds, 0.02f, 4, 3, borderColor);

    if (!title.empty()) {
        DrawRectangleRounded({bounds.x + 2, bounds.y + 2, bounds.width - 4, 40}, 0.1f, 4,
                             {borderColor.r, borderColor.g, borderColor.b, 50});
        DrawText(title.c_str(), static_cast<int>(bounds.x + 15), static_cast<int>(bounds.y + 10), 20, borderColor);
    }
}

void UIComponents::label(Vector2 pos, const std::string& text, int fontSize, Color color) {
    DrawText(text.c_str(), static_cast<int>(pos.x), static_cast<int>(pos.y), fontSize, color);
}

void UIComponents::labelCentered(Rectangle bounds, const std::string& text, int fontSize, Color color) {
    int textWidth = MeasureText(text.c_str(), fontSize);
    float x = bounds.x + (bounds.width - textWidth) / 2;
    float y = bounds.y + (bounds.height - fontSize) / 2;
    DrawText(text.c_str(), static_cast<int>(x), static_cast<int>(y), fontSize, color);
}

void UIComponents::title(Vector2 pos, const std::string& text) {
    DrawText(text.c_str(), static_cast<int>(pos.x), static_cast<int>(pos.y), 36, Colors::TEXT_HIGHLIGHT);
}

void UIComponents::subtitle(Vector2 pos, const std::string& text) {
    DrawText(text.c_str(), static_cast<int>(pos.x), static_cast<int>(pos.y), 24, Colors::PRIMARY_LIGHT);
}

void UIComponents::paragraph(Rectangle bounds, const std::string& text, int fontSize) {
    // Einfacher Textumbruch
    std::string word;
    std::string line;
    float y = bounds.y;
    float lineHeight = fontSize * 1.5f;
    float maxWidth = bounds.width;

    for (size_t i = 0; i <= text.size(); ++i) {
        char c = (i < text.size()) ? text[i] : ' ';

        if (c == ' ' || c == '\n' || i == text.size()) {
            std::string testLine = line.empty() ? word : line + " " + word;
            int testWidth = MeasureText(testLine.c_str(), fontSize);

            if (testWidth > maxWidth && !line.empty()) {
                DrawText(line.c_str(), static_cast<int>(bounds.x), static_cast<int>(y), fontSize, Colors::TEXT);
                y += lineHeight;
                line = word;
            } else {
                line = testLine;
            }
            word.clear();

            if (c == '\n') {
                DrawText(line.c_str(), static_cast<int>(bounds.x), static_cast<int>(y), fontSize, Colors::TEXT);
                y += lineHeight;
                line.clear();
            }
        } else {
            word += c;
        }
    }

    if (!line.empty()) {
        DrawText(line.c_str(), static_cast<int>(bounds.x), static_cast<int>(y), fontSize, Colors::TEXT);
    }
}

void UIComponents::codeBlock(Rectangle bounds, const std::string& code, const std::string& language) {
    // Hintergrund
    DrawRectangleRounded(bounds, 0.02f, 4, {20, 20, 30, 255});
    DrawRectangleRoundedLines(bounds, 0.02f, 4, 1, Colors::BORDER);

    // Sprache-Label
    if (!language.empty()) {
        DrawText(language.c_str(), static_cast<int>(bounds.x + bounds.width - 50),
                 static_cast<int>(bounds.y + 5), 12, Colors::TEXT_DIM);
    }

    // Code mit Syntax-Highlighting (vereinfacht)
    int fontSize = 16;
    float lineHeight = fontSize * 1.4f;
    float x = bounds.x + 15;
    float y = bounds.y + 25;
    int lineNum = 1;

    std::string line;
    for (char c : code) {
        if (c == '\n') {
            // Zeilennummer
            DrawText(TextFormat("%2d", lineNum), static_cast<int>(x - 5), static_cast<int>(y), fontSize, Colors::TEXT_DIM);

            // Einfaches Syntax-Highlighting
            Color lineColor = Colors::TEXT;
            if (line.find("//") != std::string::npos) {
                lineColor = Colors::ACCENT_GREEN;
            } else if (line.find("Tensor") != std::string::npos) {
                lineColor = Colors::PRIMARY_LIGHT;
            } else if (line.find("int ") != std::string::npos || line.find("float ") != std::string::npos ||
                       line.find("auto ") != std::string::npos) {
                lineColor = Colors::ACCENT_PURPLE;
            }

            DrawText(line.c_str(), static_cast<int>(x + 30), static_cast<int>(y), fontSize, lineColor);
            y += lineHeight;
            lineNum++;
            line.clear();

            if (y > bounds.y + bounds.height - 20) break;
        } else {
            line += c;
        }
    }

    // Letzte Zeile
    if (!line.empty() && y <= bounds.y + bounds.height - 20) {
        DrawText(TextFormat("%2d", lineNum), static_cast<int>(x - 5), static_cast<int>(y), fontSize, Colors::TEXT_DIM);
        DrawText(line.c_str(), static_cast<int>(x + 30), static_cast<int>(y), fontSize, Colors::TEXT);
    }
}

void UIComponents::codeBlockAnimated(Rectangle bounds, const std::string& code, float progress) {
    size_t visibleChars = static_cast<size_t>(progress * code.size());
    std::string visibleCode = code.substr(0, visibleChars);
    codeBlock(bounds, visibleCode, "cpp");

    // Cursor-Effekt
    if (progress < 1.0f && static_cast<int>(GetTime() * 2) % 2 == 0) {
        // Berechne Cursor-Position (vereinfacht)
        int lines = 1;
        int lastLineLen = 0;
        for (char c : visibleCode) {
            if (c == '\n') { lines++; lastLineLen = 0; }
            else lastLineLen++;
        }
        float cursorX = bounds.x + 45 + lastLineLen * 9.6f;
        float cursorY = bounds.y + 25 + (lines - 1) * 22.4f;
        DrawRectangle(static_cast<int>(cursorX), static_cast<int>(cursorY), 2, 16, Colors::PRIMARY);
    }
}

void UIComponents::progressBar(Rectangle bounds, float progress, const std::string& label) {
    progress = std::clamp(progress, 0.0f, 1.0f);

    // Hintergrund
    DrawRectangleRounded(bounds, 0.5f, 8, Colors::BACKGROUND_LIGHT);

    // Fortschritt
    Rectangle fillBounds = {bounds.x + 2, bounds.y + 2, (bounds.width - 4) * progress, bounds.height - 4};
    if (fillBounds.width > 0) {
        DrawRectangleRounded(fillBounds, 0.5f, 8, Colors::PRIMARY);
    }

    // Label
    if (!label.empty()) {
        labelCentered(bounds, label, 14, Colors::TEXT);
    }
}

void UIComponents::stepIndicator(Rectangle bounds, int currentStep, int totalSteps, const std::vector<std::string>& labels) {
    float stepWidth = bounds.width / totalSteps;

    for (int i = 0; i < totalSteps; ++i) {
        float x = bounds.x + i * stepWidth + stepWidth / 2;
        float y = bounds.y + bounds.height / 2;

        // Verbindungslinie
        if (i < totalSteps - 1) {
            Color lineColor = (i < currentStep) ? Colors::PRIMARY : Colors::BORDER;
            DrawLineEx({x + 15, y}, {x + stepWidth - 15, y}, 2, lineColor);
        }

        // Kreis
        Color circleColor = (i <= currentStep) ? Colors::PRIMARY : Colors::BACKGROUND_LIGHT;
        DrawCircle(static_cast<int>(x), static_cast<int>(y), 15, circleColor);
        DrawCircleLines(static_cast<int>(x), static_cast<int>(y), 15, Colors::BORDER);

        // Nummer
        DrawText(TextFormat("%d", i + 1), static_cast<int>(x - 5), static_cast<int>(y - 8), 16,
                 (i <= currentStep) ? Colors::TEXT_HIGHLIGHT : Colors::TEXT_DIM);

        // Label
        if (i < static_cast<int>(labels.size())) {
            int labelWidth = MeasureText(labels[i].c_str(), 12);
            DrawText(labels[i].c_str(), static_cast<int>(x - labelWidth / 2), static_cast<int>(y + 25), 12,
                     (i == currentStep) ? Colors::TEXT : Colors::TEXT_DIM);
        }
    }
}

int UIComponents::listBox(Rectangle bounds, const std::vector<std::string>& items, int selectedIndex) {
    panel(bounds, "");

    float itemHeight = 35;
    int newSelected = selectedIndex;

    for (size_t i = 0; i < items.size(); ++i) {
        Rectangle itemBounds = {bounds.x + 5, bounds.y + 5 + i * itemHeight, bounds.width - 10, itemHeight - 2};

        if (itemBounds.y + itemHeight > bounds.y + bounds.height) break;

        Vector2 mouse = GetMousePosition();
        bool hover = CheckCollisionPointRec(mouse, itemBounds);
        bool selected = (static_cast<int>(i) == selectedIndex);

        if (hover || selected) {
            DrawRectangleRounded(itemBounds, 0.2f, 4, selected ? Colors::PRIMARY_DARK : Colors::BUTTON_HOVER);
        }

        if (hover && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            newSelected = static_cast<int>(i);
        }

        DrawText(items[i].c_str(), static_cast<int>(itemBounds.x + 10),
                 static_cast<int>(itemBounds.y + (itemHeight - 18) / 2), 18, Colors::TEXT);
    }

    return newSelected;
}

void UIComponents::infoCard(Rectangle bounds, const std::string& title, const std::string& content, Color accentColor) {
    panelWithBorder(bounds, title, accentColor);

    Rectangle textBounds = {bounds.x + 15, bounds.y + 55, bounds.width - 30, bounds.height - 70};
    paragraph(textBounds, content, 16);
}

bool UIComponents::textInput(Rectangle bounds, std::string& text, const std::string& placeholder) {
    static bool active = false;
    static Rectangle activeBox = {0, 0, 0, 0};

    Vector2 mouse = GetMousePosition();
    bool hover = CheckCollisionPointRec(mouse, bounds);

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        active = hover;
        activeBox = bounds;
    }

    bool isActive = active && (activeBox.x == bounds.x && activeBox.y == bounds.y);

    // Hintergrund
    Color bgColor = isActive ? Colors::BACKGROUND_LIGHT : Colors::BACKGROUND_PANEL;
    DrawRectangleRounded(bounds, 0.1f, 4, bgColor);
    DrawRectangleRoundedLines(bounds, 0.1f, 4, 2, isActive ? Colors::PRIMARY : Colors::BORDER);

    // Text oder Placeholder
    std::string displayText = text.empty() ? placeholder : text;
    Color textColor = text.empty() ? Colors::TEXT_DIM : Colors::TEXT;
    DrawText(displayText.c_str(), static_cast<int>(bounds.x + 10),
             static_cast<int>(bounds.y + (bounds.height - 18) / 2), 18, textColor);

    // Cursor
    if (isActive && static_cast<int>(GetTime() * 2) % 2 == 0) {
        int textWidth = MeasureText(text.c_str(), 18);
        DrawRectangle(static_cast<int>(bounds.x + 12 + textWidth),
                      static_cast<int>(bounds.y + bounds.height / 2 - 10), 2, 20, Colors::PRIMARY);
    }

    // Eingabe verarbeiten
    if (isActive) {
        int key = GetCharPressed();
        while (key > 0) {
            if (key >= 32 && key <= 125 && text.size() < 100) {
                text += static_cast<char>(key);
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && !text.empty()) {
            text.pop_back();
        }

        if (IsKeyPressed(KEY_ENTER)) {
            return true;
        }
    }

    return false;
}

float UIComponents::slider(Rectangle bounds, float value, float min, float max, const std::string& label) {
    float range = max - min;
    float normalizedValue = (value - min) / range;

    // Label
    if (!label.empty()) {
        DrawText(label.c_str(), static_cast<int>(bounds.x), static_cast<int>(bounds.y - 25), 16, Colors::TEXT);
        DrawText(TextFormat("%.2f", value), static_cast<int>(bounds.x + bounds.width - 50),
                 static_cast<int>(bounds.y - 25), 16, Colors::PRIMARY_LIGHT);
    }

    // Track
    DrawRectangleRounded(bounds, 0.5f, 8, Colors::BACKGROUND_LIGHT);

    // Fill
    Rectangle fill = {bounds.x, bounds.y, bounds.width * normalizedValue, bounds.height};
    DrawRectangleRounded(fill, 0.5f, 8, Colors::PRIMARY_DARK);

    // Handle
    float handleX = bounds.x + bounds.width * normalizedValue;
    DrawCircle(static_cast<int>(handleX), static_cast<int>(bounds.y + bounds.height / 2), 12, Colors::PRIMARY);
    DrawCircleLines(static_cast<int>(handleX), static_cast<int>(bounds.y + bounds.height / 2), 12, Colors::TEXT);

    // Interaktion
    Vector2 mouse = GetMousePosition();
    if (CheckCollisionPointRec(mouse, {bounds.x - 10, bounds.y - 10, bounds.width + 20, bounds.height + 20})) {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            normalizedValue = std::clamp((mouse.x - bounds.x) / bounds.width, 0.0f, 1.0f);
            return min + normalizedValue * range;
        }
    }

    return value;
}

void UIComponents::tooltip(const std::string& text) {
    if (!currentTooltip_.empty()) {
        showTooltipAt(tooltipPos_, currentTooltip_);
    }
    currentTooltip_ = text;
}

void UIComponents::showTooltipAt(Vector2 pos, const std::string& text) {
    int padding = 8;
    int fontSize = 14;
    int width = MeasureText(text.c_str(), fontSize) + padding * 2;
    int height = fontSize + padding * 2;

    // Position anpassen
    if (pos.x + width > screenWidth_) pos.x = screenWidth_ - width - 5;
    if (pos.y + height > screenHeight_) pos.y = screenHeight_ - height - 5;

    DrawRectangleRounded({pos.x, pos.y, static_cast<float>(width), static_cast<float>(height)}, 0.2f, 4, {40, 40, 60, 240});
    DrawText(text.c_str(), static_cast<int>(pos.x + padding), static_cast<int>(pos.y + padding), fontSize, Colors::TEXT);
}

void UIComponents::notification(const std::string& message, Color color) {
    notifications_.push_back({message, 3.0f});
}

void UIComponents::successMessage(const std::string& message) {
    notification(message, Colors::ACCENT_GREEN);
}

void UIComponents::errorMessage(const std::string& message) {
    notification(message, Colors::ACCENT);
}

int UIComponents::tabBar(Rectangle bounds, const std::vector<std::string>& tabs, int selectedTab) {
    float tabWidth = bounds.width / tabs.size();
    int newSelected = selectedTab;

    DrawRectangle(static_cast<int>(bounds.x), static_cast<int>(bounds.y), static_cast<int>(bounds.width), static_cast<int>(bounds.height), Colors::BACKGROUND_LIGHT);

    for (size_t i = 0; i < tabs.size(); ++i) {
        Rectangle tabBounds = {bounds.x + i * tabWidth, bounds.y, tabWidth, bounds.height};
        bool selected = (static_cast<int>(i) == selectedTab);

        Vector2 mouse = GetMousePosition();
        bool hover = CheckCollisionPointRec(mouse, tabBounds);

        if (selected) {
            DrawRectangle(static_cast<int>(tabBounds.x), static_cast<int>(tabBounds.y),
                          static_cast<int>(tabBounds.width), static_cast<int>(tabBounds.height), Colors::BACKGROUND_PANEL);
            DrawRectangle(static_cast<int>(tabBounds.x), static_cast<int>(tabBounds.y + tabBounds.height - 3),
                          static_cast<int>(tabBounds.width), 3, Colors::PRIMARY);
        } else if (hover) {
            DrawRectangle(static_cast<int>(tabBounds.x), static_cast<int>(tabBounds.y),
                          static_cast<int>(tabBounds.width), static_cast<int>(tabBounds.height), Colors::BUTTON_HOVER);
        }

        if (hover && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            newSelected = static_cast<int>(i);
        }

        labelCentered(tabBounds, tabs[i], 18, selected ? Colors::TEXT_HIGHLIGHT : Colors::TEXT_DIM);
    }

    return newSelected;
}

bool UIComponents::backButton(Vector2 pos) {
    return button({pos.x, pos.y, 100, 40}, "< Zurueck");
}

bool UIComponents::nextButton(Vector2 pos, const std::string& text) {
    return button({pos.x, pos.y, 150, 45}, text + " >");
}

float UIComponents::easeInOut(float t) {
    return t < 0.5f ? 2 * t * t : 1 - std::pow(-2 * t + 2, 2) / 2;
}

float UIComponents::easeOut(float t) {
    return 1 - std::pow(1 - t, 3);
}

Color UIComponents::lerpColor(Color a, Color b, float t) {
    return {
        static_cast<unsigned char>(a.r + (b.r - a.r) * t),
        static_cast<unsigned char>(a.g + (b.g - a.g) * t),
        static_cast<unsigned char>(a.b + (b.b - a.b) * t),
        static_cast<unsigned char>(a.a + (b.a - a.a) * t)
    };
}

void UIComponents::updateAnimations(float deltaTime) {
    // Update notifications
    for (auto it = notifications_.begin(); it != notifications_.end();) {
        it->second -= deltaTime;
        if (it->second <= 0) {
            it = notifications_.erase(it);
        } else {
            ++it;
        }
    }

    // Draw notifications
    float y = 20;
    for (const auto& [msg, time] : notifications_) {
        float alpha = std::min(1.0f, time);
        int width = MeasureText(msg.c_str(), 18) + 30;
        float x = screenWidth_ - width - 20;

        DrawRectangleRounded({x, y, static_cast<float>(width), 40}, 0.3f, 4,
                             {Colors::ACCENT_GREEN.r, Colors::ACCENT_GREEN.g, Colors::ACCENT_GREEN.b, static_cast<unsigned char>(200 * alpha)});
        DrawText(msg.c_str(), static_cast<int>(x + 15), static_cast<int>(y + 10), 18,
                 {255, 255, 255, static_cast<unsigned char>(255 * alpha)});
        y += 50;
    }

    currentTooltip_.clear();
}

// === TypeWriter ===

TypeWriter::TypeWriter(const std::string& text, float charsPerSecond)
    : fullText_(text), charsPerSecond_(charsPerSecond), currentChar_(0), complete_(false) {}

void TypeWriter::start(const std::string& text) {
    fullText_ = text;
    currentChar_ = 0;
    complete_ = false;
}

void TypeWriter::update(float deltaTime) {
    if (!complete_) {
        currentChar_ += charsPerSecond_ * deltaTime;
        if (currentChar_ >= fullText_.size()) {
            currentChar_ = static_cast<float>(fullText_.size());
            complete_ = true;
        }
    }
}

void TypeWriter::skip() {
    currentChar_ = static_cast<float>(fullText_.size());
    complete_ = true;
}

bool TypeWriter::isComplete() const {
    return complete_;
}

std::string TypeWriter::getCurrentText() const {
    return fullText_.substr(0, static_cast<size_t>(currentChar_));
}

float TypeWriter::getProgress() const {
    return fullText_.empty() ? 1.0f : currentChar_ / fullText_.size();
}

// === ScrollPanel ===

ScrollPanel::ScrollPanel(Rectangle bounds) : bounds_(bounds) {}

void ScrollPanel::begin() {
    BeginScissorMode(static_cast<int>(bounds_.x), static_cast<int>(bounds_.y),
                     static_cast<int>(bounds_.width), static_cast<int>(bounds_.height));
}

void ScrollPanel::end() {
    EndScissorMode();

    // Scrollbar
    if (contentHeight_ > bounds_.height) {
        float scrollbarHeight = bounds_.height * (bounds_.height / contentHeight_);
        float scrollbarY = bounds_.y + (scrollOffset_ / (contentHeight_ - bounds_.height)) * (bounds_.height - scrollbarHeight);

        DrawRectangle(static_cast<int>(bounds_.x + bounds_.width - 8), static_cast<int>(bounds_.y), 8, static_cast<int>(bounds_.height), Colors::BACKGROUND_LIGHT);
        DrawRectangleRounded({bounds_.x + bounds_.width - 6, scrollbarY, 4, scrollbarHeight}, 0.5f, 4, Colors::PRIMARY);

        // Scroll mit Mausrad
        Vector2 mouse = GetMousePosition();
        if (CheckCollisionPointRec(mouse, bounds_)) {
            float wheel = GetMouseWheelMove();
            scrollOffset_ = std::clamp(scrollOffset_ - wheel * 50, 0.0f, contentHeight_ - bounds_.height);
        }
    }
}

void ScrollPanel::setContentHeight(float height) {
    contentHeight_ = height;
}

} // namespace gui
