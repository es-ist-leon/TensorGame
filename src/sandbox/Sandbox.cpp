#include "sandbox/Sandbox.hpp"
#include <sstream>
#include <algorithm>

namespace sandbox {

Sandbox::Sandbox() {
    // Beispiel-Tensor erstellen
    currentTensor_ = tensor::Tensor::random({3, 3, 3});
    hasTensor_ = true;
    visualizer_.setTensor(currentTensor_, "tensor");

    // Beispiele in Storage
    storage_.store("beispiel", tensor::Tensor::range(1, 6), "Beispiel-Vektor");
    storage_.store("matrix", tensor::Tensor::identity(3), "Einheitsmatrix");
}

void Sandbox::update(float deltaTime) {
    visualizer_.update(deltaTime);
}

void Sandbox::render() {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    // Layout
    float vizX = controlPanelWidth_ + 20;
    float vizWidth = screenWidth - controlPanelWidth_ - 40;
    float vizHeight = screenHeight - consoleHeight_ - 140;

    // Control Panel (links)
    renderControlPanel();

    // Visualisierung (rechts oben)
    Rectangle vizBounds = {vizX, 80, vizWidth, vizHeight};
    DrawRectangleRounded(vizBounds, 0.01f, 4, gui::Colors::BACKGROUND_PANEL);
    DrawRectangleRoundedLines(vizBounds, 0.01f, 4, 1, gui::Colors::BORDER);

    BeginScissorMode(static_cast<int>(vizBounds.x), static_cast<int>(vizBounds.y),
                     static_cast<int>(vizBounds.width), static_cast<int>(vizBounds.height));
    visualizer_.render3D();
    EndScissorMode();

    // Tensor-Info Overlay
    renderTensorInfo();

    // Quick Actions
    renderQuickActions();

    // Konsole (unten)
    renderConsole();

    // Visualisierungsmodus-Auswahl
    const char* modes[] = {"Wuerfel", "Kugeln", "Punkte", "Heatmap", "Balken", "Netzwerk", "Speicher"};
    float modeX = vizX + 10;
    float modeY = vizBounds.y + vizBounds.height - 45;

    for (int i = 0; i < 7; ++i) {
        Rectangle btn = {modeX + i * 85, modeY, 80, 35};
        bool selected = (static_cast<int>(currentMode_) == i);

        Color bgColor = selected ? gui::Colors::PRIMARY : gui::Colors::BUTTON;
        if (CheckCollisionPointRec(GetMousePosition(), btn)) {
            bgColor = gui::Colors::BUTTON_HOVER;
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                currentMode_ = static_cast<gui::VisualizationMode>(i);
                visualizer_.setMode(currentMode_);
            }
        }

        DrawRectangleRounded(btn, 0.3f, 4, bgColor);
        gui::UIComponents::labelCentered(btn, modes[i], 12, gui::Colors::TEXT);
    }
}

void Sandbox::renderControlPanel() {
    Rectangle panelBounds = {10, 80, controlPanelWidth_, static_cast<float>(GetScreenHeight()) - 100};

    gui::UIComponents::panel(panelBounds, "Tensor-Werkzeuge");

    // Tabs
    std::vector<std::string> tabs = {"Erstellen", "Operationen", "Speicher"};
    selectedTab_ = gui::UIComponents::tabBar(
        {panelBounds.x + 5, panelBounds.y + 45, panelBounds.width - 10, 35},
        tabs, selectedTab_
    );

    float contentY = panelBounds.y + 90;
    float contentX = panelBounds.x + 15;
    float contentWidth = panelBounds.width - 30;

    if (selectedTab_ == 0) {
        // === ERSTELLEN TAB ===
        DrawText("Shape (z.B. 3,3,3):", static_cast<int>(contentX), static_cast<int>(contentY), 16, gui::Colors::TEXT);
        contentY += 25;

        Rectangle shapeInputBounds = {contentX, contentY, contentWidth, 35};
        gui::UIComponents::textInput(shapeInputBounds, shapeInput_, "3,3");
        contentY += 50;

        // Erstell-Buttons
        if (gui::UIComponents::button({contentX, contentY, contentWidth, 35}, "Nullen erstellen")) {
            createZeros();
        }
        contentY += 45;

        if (gui::UIComponents::button({contentX, contentY, contentWidth, 35}, "Einsen erstellen")) {
            createOnes();
        }
        contentY += 45;

        if (gui::UIComponents::button({contentX, contentY, contentWidth, 35}, "Zufaellig erstellen")) {
            createRandom();
        }
        contentY += 55;

        // Range
        DrawText("Bereich:", static_cast<int>(contentX), static_cast<int>(contentY), 16, gui::Colors::TEXT);
        contentY += 25;

        rangeStart_ = gui::UIComponents::slider({contentX, contentY, contentWidth, 15}, rangeStart_, -10, 10, "Start");
        contentY += 45;
        rangeEnd_ = gui::UIComponents::slider({contentX, contentY, contentWidth, 15}, rangeEnd_, -10, 20, "Ende");
        contentY += 45;

        if (gui::UIComponents::button({contentX, contentY, contentWidth, 35}, "Range erstellen")) {
            createRange();
        }
        contentY += 55;

        // Identity
        DrawText("Einheitsmatrix:", static_cast<int>(contentX), static_cast<int>(contentY), 16, gui::Colors::TEXT);
        contentY += 25;

        identitySize_ = static_cast<int>(gui::UIComponents::slider({contentX, contentY, contentWidth, 15},
            static_cast<float>(identitySize_), 2, 8, "Groesse"));
        contentY += 45;

        if (gui::UIComponents::button({contentX, contentY, contentWidth, 35}, "Identity erstellen")) {
            createIdentity();
        }

    } else if (selectedTab_ == 1) {
        // === OPERATIONEN TAB ===
        DrawText("Skalar-Operationen:", static_cast<int>(contentX), static_cast<int>(contentY), 16, gui::Colors::TEXT);
        contentY += 25;

        scalarValue_ = gui::UIComponents::slider({contentX, contentY, contentWidth, 15}, scalarValue_, -10, 10, "Wert");
        contentY += 50;

        float halfWidth = (contentWidth - 10) / 2;

        if (gui::UIComponents::button({contentX, contentY, halfWidth, 35}, "+ Addieren")) {
            if (hasTensor_) {
                currentTensor_ = currentTensor_ + scalarValue_;
                visualizer_.setTensor(currentTensor_, currentTensorName_);
            }
        }
        if (gui::UIComponents::button({contentX + halfWidth + 10, contentY, halfWidth, 35}, "- Subtrahieren")) {
            if (hasTensor_) {
                currentTensor_ = currentTensor_ - scalarValue_;
                visualizer_.setTensor(currentTensor_, currentTensorName_);
            }
        }
        contentY += 45;

        if (gui::UIComponents::button({contentX, contentY, halfWidth, 35}, "* Multiplizieren")) {
            if (hasTensor_) {
                currentTensor_ = currentTensor_ * scalarValue_;
                visualizer_.setTensor(currentTensor_, currentTensorName_);
            }
        }
        if (gui::UIComponents::button({contentX + halfWidth + 10, contentY, halfWidth, 35}, "/ Dividieren")) {
            if (hasTensor_ && scalarValue_ != 0) {
                currentTensor_ = currentTensor_ / scalarValue_;
                visualizer_.setTensor(currentTensor_, currentTensorName_);
            }
        }
        contentY += 55;

        // Unäre Operationen
        DrawText("Transformationen:", static_cast<int>(contentX), static_cast<int>(contentY), 16, gui::Colors::TEXT);
        contentY += 25;

        if (gui::UIComponents::button({contentX, contentY, halfWidth, 35}, "Transponieren")) {
            if (hasTensor_ && currentTensor_.rank() == 2) {
                currentTensor_ = currentTensor_.transpose();
                visualizer_.setTensor(currentTensor_, currentTensorName_);
            }
        }
        if (gui::UIComponents::button({contentX + halfWidth + 10, contentY, halfWidth, 35}, "Flatten")) {
            if (hasTensor_) {
                currentTensor_ = currentTensor_.flatten();
                visualizer_.setTensor(currentTensor_, currentTensorName_);
            }
        }
        contentY += 45;

        if (gui::UIComponents::button({contentX, contentY, halfWidth, 35}, "Sqrt")) {
            if (hasTensor_) {
                currentTensor_ = currentTensor_.abs().sqrt();
                visualizer_.setTensor(currentTensor_, currentTensorName_);
            }
        }
        if (gui::UIComponents::button({contentX + halfWidth + 10, contentY, halfWidth, 35}, "Abs")) {
            if (hasTensor_) {
                currentTensor_ = currentTensor_.abs();
                visualizer_.setTensor(currentTensor_, currentTensorName_);
            }
        }
        contentY += 45;

        if (gui::UIComponents::button({contentX, contentY, halfWidth, 35}, "Normalisieren")) {
            if (hasTensor_) {
                currentTensor_ = currentTensor_.normalize();
                visualizer_.setTensor(currentTensor_, currentTensorName_);
            }
        }
        if (gui::UIComponents::button({contentX + halfWidth + 10, contentY, halfWidth, 35}, "Negieren")) {
            if (hasTensor_) {
                currentTensor_ = -currentTensor_;
                visualizer_.setTensor(currentTensor_, currentTensorName_);
            }
        }

    } else {
        // === SPEICHER TAB ===
        DrawText("Gespeicherte Tensoren:", static_cast<int>(contentX), static_cast<int>(contentY), 16, gui::Colors::TEXT);
        contentY += 30;

        auto names = storage_.listNames();
        for (const auto& name : names) {
            Rectangle itemBounds = {contentX, contentY, contentWidth, 40};

            bool hover = CheckCollisionPointRec(GetMousePosition(), itemBounds);
            if (hover) {
                DrawRectangleRounded(itemBounds, 0.2f, 4, gui::Colors::BUTTON_HOVER);
            }

            auto meta = storage_.getMetadata(name);
            DrawText(name.c_str(), static_cast<int>(contentX + 10), static_cast<int>(contentY + 5), 16, gui::Colors::TEXT);
            if (meta) {
                DrawText(meta->shapeString().c_str(), static_cast<int>(contentX + 10), static_cast<int>(contentY + 22), 12, gui::Colors::TEXT_DIM);
            }

            // Laden-Button
            Rectangle loadBtn = {contentX + contentWidth - 60, contentY + 5, 50, 30};
            if (gui::UIComponents::button(loadBtn, "Laden")) {
                auto t = storage_.get(name);
                if (t) {
                    currentTensor_ = *t;
                    currentTensorName_ = name;
                    hasTensor_ = true;
                    visualizer_.setTensor(currentTensor_, currentTensorName_);
                }
            }

            contentY += 45;
        }

        contentY += 20;

        // Aktuellen speichern
        static std::string saveName = "neuer_tensor";
        DrawText("Aktuellen speichern:", static_cast<int>(contentX), static_cast<int>(contentY), 16, gui::Colors::TEXT);
        contentY += 25;

        gui::UIComponents::textInput({contentX, contentY, contentWidth - 80, 35}, saveName, "Name");

        if (gui::UIComponents::button({contentX + contentWidth - 70, contentY, 70, 35}, "Speichern")) {
            if (hasTensor_ && !saveName.empty()) {
                storage_.store(saveName, currentTensor_);
            }
        }
    }
}

void Sandbox::renderTensorInfo() {
    if (!hasTensor_) return;

    float infoX = controlPanelWidth_ + 30;
    float infoY = 90;

    DrawRectangle(static_cast<int>(infoX), static_cast<int>(infoY), 250, 100, {30, 30, 50, 220});

    DrawText(currentTensorName_.c_str(), static_cast<int>(infoX + 10), static_cast<int>(infoY + 10), 20, gui::Colors::TEXT_HIGHLIGHT);
    DrawText(TextFormat("Shape: %s", currentTensor_.shapeString().c_str()), static_cast<int>(infoX + 10), static_cast<int>(infoY + 35), 16, gui::Colors::TEXT);
    DrawText(TextFormat("Rang: %zu | Elemente: %zu", currentTensor_.rank(), currentTensor_.size()), static_cast<int>(infoX + 10), static_cast<int>(infoY + 55), 14, gui::Colors::TEXT_DIM);

    if (!currentTensor_.empty()) {
        DrawText(TextFormat("Min: %.2f | Max: %.2f | Mean: %.2f",
            currentTensor_.min(), currentTensor_.max(), currentTensor_.mean()),
            static_cast<int>(infoX + 10), static_cast<int>(infoY + 75), 12, gui::Colors::TEXT_DIM);
    }
}

void Sandbox::renderQuickActions() {
    // Quick action buttons at bottom of viz
}

void Sandbox::renderConsole() {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    float consoleX = controlPanelWidth_ + 20;
    float consoleY = screenHeight - consoleHeight_ - 10;
    float consoleWidth = screenWidth - controlPanelWidth_ - 40;

    Rectangle consoleBounds = {consoleX, consoleY, consoleWidth, consoleHeight_};

    gui::UIComponents::panel(consoleBounds, "Konsole");

    // History
    float historyY = consoleY + 45;
    for (const auto& cmd : commandHistory_) {
        Color outputColor = cmd.success ? gui::Colors::ACCENT_GREEN : gui::Colors::ACCENT;
        DrawText(TextFormat("> %s", cmd.input.c_str()), static_cast<int>(consoleX + 10), static_cast<int>(historyY), 14, gui::Colors::PRIMARY_LIGHT);
        historyY += 18;
        DrawText(cmd.output.c_str(), static_cast<int>(consoleX + 20), static_cast<int>(historyY), 14, outputColor);
        historyY += 20;

        if (historyY > consoleY + consoleHeight_ - 50) break;
    }

    // Input
    Rectangle inputBounds = {consoleX + 10, consoleY + consoleHeight_ - 45, consoleWidth - 100, 35};
    if (gui::UIComponents::textInput(inputBounds, commandInput_, "Befehl eingeben...")) {
        executeCommand(commandInput_);
        commandInput_.clear();
    }

    if (gui::UIComponents::button({consoleX + consoleWidth - 80, consoleY + consoleHeight_ - 45, 70, 35}, "Senden")) {
        executeCommand(commandInput_);
        commandInput_.clear();
    }

    // Hilfe
    DrawText("Befehle: zeros, ones, random, range, identity, +, -, *, /, transpose, sqrt, help",
             static_cast<int>(consoleX + 10), static_cast<int>(consoleY + consoleHeight_ - 12), 11, gui::Colors::TEXT_DIM);
}

void Sandbox::createTensor() {
    // Parse shape from input
    std::vector<size_t> shape;
    std::stringstream ss(shapeInput_);
    std::string token;

    while (std::getline(ss, token, ',')) {
        try {
            int dim = std::stoi(token);
            if (dim > 0) shape.push_back(static_cast<size_t>(dim));
        } catch (...) {}
    }

    if (!shape.empty()) {
        currentTensor_ = tensor::Tensor(shape);
        hasTensor_ = true;
        visualizer_.setTensor(currentTensor_, currentTensorName_);
    }
}

void Sandbox::createRandom() {
    std::vector<size_t> shape;
    std::stringstream ss(shapeInput_);
    std::string token;

    while (std::getline(ss, token, ',')) {
        try {
            int dim = std::stoi(token);
            if (dim > 0) shape.push_back(static_cast<size_t>(dim));
        } catch (...) {}
    }

    if (!shape.empty()) {
        currentTensor_ = tensor::Tensor::random(shape, randomMin_, randomMax_);
        hasTensor_ = true;
        visualizer_.setTensor(currentTensor_, currentTensorName_);
    }
}

void Sandbox::createZeros() {
    std::vector<size_t> shape;
    std::stringstream ss(shapeInput_);
    std::string token;

    while (std::getline(ss, token, ',')) {
        try {
            int dim = std::stoi(token);
            if (dim > 0) shape.push_back(static_cast<size_t>(dim));
        } catch (...) {}
    }

    if (!shape.empty()) {
        currentTensor_ = tensor::Tensor::zeros(shape);
        hasTensor_ = true;
        visualizer_.setTensor(currentTensor_, currentTensorName_);
    }
}

void Sandbox::createOnes() {
    std::vector<size_t> shape;
    std::stringstream ss(shapeInput_);
    std::string token;

    while (std::getline(ss, token, ',')) {
        try {
            int dim = std::stoi(token);
            if (dim > 0) shape.push_back(static_cast<size_t>(dim));
        } catch (...) {}
    }

    if (!shape.empty()) {
        currentTensor_ = tensor::Tensor::ones(shape);
        hasTensor_ = true;
        visualizer_.setTensor(currentTensor_, currentTensorName_);
    }
}

void Sandbox::createIdentity() {
    currentTensor_ = tensor::Tensor::identity(static_cast<size_t>(identitySize_));
    hasTensor_ = true;
    visualizer_.setTensor(currentTensor_, currentTensorName_);
}

void Sandbox::createRange() {
    currentTensor_ = tensor::Tensor::range(rangeStart_, rangeEnd_, rangeStep_);
    hasTensor_ = true;
    visualizer_.setTensor(currentTensor_, currentTensorName_);
}

void Sandbox::executeCommand(const std::string& cmd) {
    if (cmd.empty()) return;

    std::string output = parseAndExecute(cmd);
    bool success = !output.empty() && output[0] != 'E'; // Error starts with E

    commandHistory_.push_front({cmd, output, success});
    if (commandHistory_.size() > maxHistorySize_) {
        commandHistory_.pop_back();
    }
}

std::string Sandbox::parseAndExecute(const std::string& cmd) {
    std::stringstream ss(cmd);
    std::string command;
    ss >> command;

    // To lowercase
    std::transform(command.begin(), command.end(), command.begin(), ::tolower);

    try {
        if (command == "help") {
            return "Befehle: zeros(shape), ones(shape), random(shape), range(start,end), identity(n), "
                   "+n, -n, *n, /n, transpose, flatten, sqrt, abs, normalize, info";
        }
        else if (command == "info") {
            if (!hasTensor_) return "Kein Tensor geladen";
            return "Shape: " + currentTensor_.shapeString() +
                   " | Elemente: " + std::to_string(currentTensor_.size()) +
                   " | Min: " + std::to_string(currentTensor_.min()) +
                   " | Max: " + std::to_string(currentTensor_.max());
        }
        else if (command == "zeros" || command == "ones" || command == "random") {
            std::string shapeStr;
            ss >> shapeStr;
            shapeInput_ = shapeStr;

            if (command == "zeros") createZeros();
            else if (command == "ones") createOnes();
            else createRandom();

            return "Tensor erstellt: " + currentTensor_.shapeString();
        }
        else if (command == "range") {
            float start, end;
            ss >> start >> end;
            rangeStart_ = start;
            rangeEnd_ = end;
            createRange();
            return "Range erstellt: " + currentTensor_.shapeString();
        }
        else if (command == "identity") {
            int n;
            ss >> n;
            identitySize_ = n;
            createIdentity();
            return "Einheitsmatrix erstellt: " + currentTensor_.shapeString();
        }
        else if (command == "transpose") {
            if (!hasTensor_) return "Fehler: Kein Tensor";
            if (currentTensor_.rank() != 2) return "Fehler: Nur fuer 2D Tensoren";
            currentTensor_ = currentTensor_.transpose();
            visualizer_.setTensor(currentTensor_, currentTensorName_);
            return "Transponiert: " + currentTensor_.shapeString();
        }
        else if (command == "flatten") {
            if (!hasTensor_) return "Fehler: Kein Tensor";
            currentTensor_ = currentTensor_.flatten();
            visualizer_.setTensor(currentTensor_, currentTensorName_);
            return "Flattened: " + currentTensor_.shapeString();
        }
        else if (command == "sqrt") {
            if (!hasTensor_) return "Fehler: Kein Tensor";
            currentTensor_ = currentTensor_.abs().sqrt();
            visualizer_.setTensor(currentTensor_, currentTensorName_);
            return "Sqrt angewendet";
        }
        else if (command == "abs") {
            if (!hasTensor_) return "Fehler: Kein Tensor";
            currentTensor_ = currentTensor_.abs();
            visualizer_.setTensor(currentTensor_, currentTensorName_);
            return "Abs angewendet";
        }
        else if (command == "normalize") {
            if (!hasTensor_) return "Fehler: Kein Tensor";
            currentTensor_ = currentTensor_.normalize();
            visualizer_.setTensor(currentTensor_, currentTensorName_);
            return "Normalisiert";
        }
        else if (command[0] == '+' || command[0] == '-' || command[0] == '*' || command[0] == '/') {
            if (!hasTensor_) return "Fehler: Kein Tensor";
            float value = std::stof(command.substr(1));
            switch (command[0]) {
                case '+': currentTensor_ = currentTensor_ + value; break;
                case '-': currentTensor_ = currentTensor_ - value; break;
                case '*': currentTensor_ = currentTensor_ * value; break;
                case '/':
                    if (value == 0) return "Fehler: Division durch 0";
                    currentTensor_ = currentTensor_ / value;
                    break;
            }
            visualizer_.setTensor(currentTensor_, currentTensorName_);
            return "Operation angewendet";
        }
        else {
            return "Unbekannter Befehl: " + command + " (help fuer Hilfe)";
        }
    } catch (const std::exception& e) {
        return std::string("Fehler: ") + e.what();
    }
}

} // namespace sandbox
