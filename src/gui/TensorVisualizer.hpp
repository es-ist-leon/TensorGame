#pragma once

#include "raylib.h"
#include "tensor/Tensor.hpp"
#include "gui/Colors.hpp"
#include <string>
#include <vector>
#include <optional>

namespace gui {

/**
 * @brief Verschiedene Visualisierungsmodi für Tensoren
 */
enum class VisualizationMode {
    Cubes,          // Würfel für jeden Wert
    Spheres,        // Kugeln für jeden Wert
    Points,         // Punkte (für große Tensoren)
    Heatmap,        // 2D Heatmap für Matrizen
    Bars,           // Balkendiagramm für Vektoren
    Network,        // Netzwerk-Darstellung
    Memory          // Speicher-Layout Ansicht
};

/**
 * @brief Informationen über ein ausgewähltes Element
 */
struct SelectedElement {
    std::vector<size_t> indices;
    float value;
    Vector3 worldPosition;
    bool valid = false;
};

/**
 * @brief 3D-Visualisierer für Tensoren
 */
class TensorVisualizer {
public:
    TensorVisualizer();

    // Tensor setzen
    void setTensor(const tensor::Tensor& t);
    void setTensor(const tensor::Tensor& t, const std::string& name);
    void clearTensor();

    // Visualisierung
    void setMode(VisualizationMode mode);
    VisualizationMode getMode() const { return mode_; }

    // Render
    void update(float deltaTime);
    void render3D();
    void renderOverlay();

    // Kamera-Kontrolle
    void updateCamera();
    void resetCamera();
    void setCameraDistance(float distance);
    void setCameraAngle(float yaw, float pitch);

    // Interaktion
    void handleInput();
    SelectedElement getSelectedElement() const { return selected_; }

    // Animationen
    void animateEntry();
    void animateValueChange(size_t index, float newValue);
    void highlightElement(const std::vector<size_t>& indices);
    void highlightSlice(size_t axis, size_t index);

    // Einstellungen
    void setSpacing(float spacing) { spacing_ = spacing; }
    void setElementSize(float size) { elementSize_ = size; }
    void setShowValues(bool show) { showValues_ = show; }
    void setShowGrid(bool show) { showGrid_ = show; }
    void setShowAxes(bool show) { showAxes_ = show; }
    void setColorByValue(bool enable) { colorByValue_ = enable; }

    // Bounds bekommen für UI-Layout
    BoundingBox getBoundingBox() const;

private:
    // Tensor-Daten
    std::optional<tensor::Tensor> tensor_;
    std::string tensorName_;
    std::vector<float> normalizedValues_;

    // Visualisierung
    VisualizationMode mode_ = VisualizationMode::Cubes;
    float spacing_ = 1.5f;
    float elementSize_ = 0.8f;
    bool showValues_ = true;
    bool showGrid_ = true;
    bool showAxes_ = true;
    bool colorByValue_ = true;

    // Kamera
    Camera3D camera_;
    float cameraYaw_ = 45.0f;
    float cameraPitch_ = 30.0f;
    float cameraDistance_ = 15.0f;
    Vector3 cameraTarget_ = {0, 0, 0};

    // Animation
    float entryAnimation_ = 0.0f;
    std::vector<float> elementAnimations_;
    std::vector<size_t> highlightedIndices_;
    float highlightPulse_ = 0.0f;

    // Interaktion
    SelectedElement selected_;
    Ray mouseRay_;

    // Hilfsfunktionen
    void drawCubesMode();
    void drawSpheresMode();
    void drawPointsMode();
    void drawHeatmapMode();
    void drawBarsMode();
    void drawNetworkMode();
    void drawMemoryMode();

    void drawGrid();
    void drawAxes();
    void drawElementInfo(Vector3 pos, const std::vector<size_t>& indices, float value);

    Vector3 getElementPosition(const std::vector<size_t>& indices) const;
    Color getElementColor(float normalizedValue, bool highlighted = false) const;

    void updateSelection();
    void updateAnimations(float deltaTime);
};

/**
 * @brief Vergleichs-Visualisierer für zwei Tensoren
 */
class TensorCompareVisualizer {
public:
    void setTensors(const tensor::Tensor& a, const tensor::Tensor& b);
    void render3D();
    void renderOverlay();

private:
    TensorVisualizer vizA_;
    TensorVisualizer vizB_;
    float separation_ = 10.0f;
};

/**
 * @brief Animations-Visualisierer für Tensor-Operationen
 */
class OperationVisualizer {
public:
    enum class Operation {
        Addition,
        Subtraction,
        Multiplication,
        Division,
        MatMul,
        Transpose,
        Reshape
    };

    void setOperation(Operation op, const tensor::Tensor& a, const tensor::Tensor& b);
    void setOperation(Operation op, const tensor::Tensor& input);

    void play();
    void pause();
    void reset();
    void setSpeed(float speed) { speed_ = speed; }

    void update(float deltaTime);
    void render3D();
    void renderOverlay();

    bool isComplete() const { return progress_ >= 1.0f; }
    float getProgress() const { return progress_; }

private:
    Operation operation_;
    tensor::Tensor tensorA_;
    tensor::Tensor tensorB_;
    tensor::Tensor result_;

    float progress_ = 0.0f;
    float speed_ = 1.0f;
    bool playing_ = false;

    TensorVisualizer vizA_;
    TensorVisualizer vizB_;
    TensorVisualizer vizResult_;
};

} // namespace gui
