#pragma once

#include "raylib.h"
#include "tensor/Tensor.hpp"
#include "tensor/TensorDB.hpp"
#include "gui/TensorVisualizer.hpp"
#include "gui/UIComponents.hpp"
#include <string>
#include <vector>
#include <deque>

namespace sandbox {

/**
 * @brief Ein Befehl im Sandbox-Verlauf
 */
struct Command {
    std::string input;
    std::string output;
    bool success;
};

/**
 * @brief Interaktiver Sandbox-Modus zum Experimentieren mit Tensoren
 */
class Sandbox {
public:
    Sandbox();

    void update(float deltaTime);
    void render();

private:
    // UI Rendering
    void renderControlPanel();
    void renderVisualization();
    void renderConsole();
    void renderTensorInfo();
    void renderQuickActions();

    // Tensor-Erstellung
    void createTensor();
    void createRandom();
    void createZeros();
    void createOnes();
    void createIdentity();
    void createRange();

    // Operationen
    void applyOperation(const std::string& op);
    void executeCommand(const std::string& cmd);
    std::string parseAndExecute(const std::string& cmd);

    // Visualisierung
    gui::TensorVisualizer visualizer_;
    gui::VisualizationMode currentMode_ = gui::VisualizationMode::Cubes;

    // Aktiver Tensor
    tensor::Tensor currentTensor_;
    std::string currentTensorName_ = "tensor";
    bool hasTensor_ = false;

    // Tensor-Speicher (Mini-DB)
    tensor::TensorDB storage_;

    // UI-Zustand
    int selectedTab_ = 0;  // 0=Erstellen, 1=Operationen, 2=Konsole

    // Erstellungs-Parameter
    std::string shapeInput_ = "3,3";
    float fillValue_ = 0.0f;
    float rangeStart_ = 0.0f;
    float rangeEnd_ = 10.0f;
    float rangeStep_ = 1.0f;
    float randomMin_ = 0.0f;
    float randomMax_ = 1.0f;
    int identitySize_ = 3;

    // Operations-Parameter
    int selectedOperation_ = 0;
    std::string operandName_ = "";
    float scalarValue_ = 1.0f;

    // Konsole
    std::string commandInput_ = "";
    std::deque<Command> commandHistory_;
    size_t maxHistorySize_ = 50;
    int historyIndex_ = -1;

    // Layout
    float controlPanelWidth_ = 350;
    float consoleHeight_ = 200;
};

} // namespace sandbox
