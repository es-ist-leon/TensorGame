#include "gui/TensorVisualizer.hpp"
#include "rlgl.h"
#include <algorithm>
#include <cmath>

namespace gui {

TensorVisualizer::TensorVisualizer() {
    // Kamera initialisieren
    camera_.position = {10.0f, 10.0f, 10.0f};
    camera_.target = {0.0f, 0.0f, 0.0f};
    camera_.up = {0.0f, 1.0f, 0.0f};
    camera_.fovy = 45.0f;
    camera_.projection = CAMERA_PERSPECTIVE;
}

void TensorVisualizer::setTensor(const tensor::Tensor& t) {
    setTensor(t, "Tensor");
}

void TensorVisualizer::setTensor(const tensor::Tensor& t, const std::string& name) {
    tensor_ = t;
    tensorName_ = name;
    normalizedValues_ = t.normalizedData();
    elementAnimations_.resize(t.size(), 0.0f);

    // Kamera anpassen
    auto bbox = getBoundingBox();
    float maxDim = std::max({bbox.max.x - bbox.min.x, bbox.max.y - bbox.min.y, bbox.max.z - bbox.min.z});
    cameraDistance_ = maxDim * 2.0f + 5.0f;
    cameraTarget_ = {(bbox.min.x + bbox.max.x) / 2, (bbox.min.y + bbox.max.y) / 2, (bbox.min.z + bbox.max.z) / 2};

    animateEntry();
}

void TensorVisualizer::clearTensor() {
    tensor_.reset();
    tensorName_.clear();
    normalizedValues_.clear();
    elementAnimations_.clear();
}

void TensorVisualizer::setMode(VisualizationMode mode) {
    mode_ = mode;
    animateEntry();
}

void TensorVisualizer::update(float deltaTime) {
    updateCamera();
    handleInput();
    updateAnimations(deltaTime);
    updateSelection();
}

void TensorVisualizer::render3D() {
    if (!tensor_) return;

    BeginMode3D(camera_);

    if (showGrid_) drawGrid();
    if (showAxes_) drawAxes();

    switch (mode_) {
        case VisualizationMode::Cubes: drawCubesMode(); break;
        case VisualizationMode::Spheres: drawSpheresMode(); break;
        case VisualizationMode::Points: drawPointsMode(); break;
        case VisualizationMode::Heatmap: drawHeatmapMode(); break;
        case VisualizationMode::Bars: drawBarsMode(); break;
        case VisualizationMode::Network: drawNetworkMode(); break;
        case VisualizationMode::Memory: drawMemoryMode(); break;
    }

    EndMode3D();
}

void TensorVisualizer::renderOverlay() {
    if (!tensor_) return;

    // Tensor-Info
    int y = 10;
    DrawText(tensorName_.c_str(), 10, y, 24, Colors::TEXT_HIGHLIGHT);
    y += 30;
    DrawText(TextFormat("Shape: %s", tensor_->shapeString().c_str()), 10, y, 18, Colors::TEXT);
    y += 25;
    DrawText(TextFormat("Rank: %zu", tensor_->rank()), 10, y, 18, Colors::TEXT);
    y += 25;
    DrawText(TextFormat("Elements: %zu", tensor_->size()), 10, y, 18, Colors::TEXT);

    // Ausgewähltes Element
    if (selected_.valid) {
        y += 40;
        DrawRectangle(5, y - 5, 200, 80, {30, 30, 50, 200});

        std::string indexStr = "[";
        for (size_t i = 0; i < selected_.indices.size(); ++i) {
            if (i > 0) indexStr += ", ";
            indexStr += std::to_string(selected_.indices[i]);
        }
        indexStr += "]";

        DrawText("Selected:", 10, y, 16, Colors::PRIMARY_LIGHT);
        y += 20;
        DrawText(TextFormat("Index: %s", indexStr.c_str()), 10, y, 16, Colors::TEXT);
        y += 20;
        DrawText(TextFormat("Value: %.4f", selected_.value), 10, y, 16, Colors::ACCENT_YELLOW);
    }

    // Steuerungshinweise
    int screenHeight = GetScreenHeight();
    DrawText("Maus: Rotieren | Scroll: Zoom | WASD: Bewegen", 10, screenHeight - 30, 14, Colors::TEXT_DIM);
}

void TensorVisualizer::updateCamera() {
    // Maussteuerung
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        Vector2 delta = GetMouseDelta();
        cameraYaw_ += delta.x * 0.3f;
        cameraPitch_ -= delta.y * 0.3f;
        cameraPitch_ = std::clamp(cameraPitch_, -89.0f, 89.0f);
    }

    // Zoom
    float wheel = GetMouseWheelMove();
    cameraDistance_ -= wheel * cameraDistance_ * 0.1f;
    cameraDistance_ = std::clamp(cameraDistance_, 2.0f, 100.0f);

    // WASD für Target-Bewegung
    float moveSpeed = 0.1f * cameraDistance_;
    if (IsKeyDown(KEY_W)) cameraTarget_.z -= moveSpeed * GetFrameTime() * 60;
    if (IsKeyDown(KEY_S)) cameraTarget_.z += moveSpeed * GetFrameTime() * 60;
    if (IsKeyDown(KEY_A)) cameraTarget_.x -= moveSpeed * GetFrameTime() * 60;
    if (IsKeyDown(KEY_D)) cameraTarget_.x += moveSpeed * GetFrameTime() * 60;
    if (IsKeyDown(KEY_Q)) cameraTarget_.y -= moveSpeed * GetFrameTime() * 60;
    if (IsKeyDown(KEY_E)) cameraTarget_.y += moveSpeed * GetFrameTime() * 60;

    // Kamera-Position berechnen
    float yawRad = cameraYaw_ * DEG2RAD;
    float pitchRad = cameraPitch_ * DEG2RAD;

    camera_.position.x = cameraTarget_.x + cameraDistance_ * cosf(pitchRad) * sinf(yawRad);
    camera_.position.y = cameraTarget_.y + cameraDistance_ * sinf(pitchRad);
    camera_.position.z = cameraTarget_.z + cameraDistance_ * cosf(pitchRad) * cosf(yawRad);
    camera_.target = cameraTarget_;
}

void TensorVisualizer::resetCamera() {
    cameraYaw_ = 45.0f;
    cameraPitch_ = 30.0f;
    cameraTarget_ = {0, 0, 0};
    if (tensor_) {
        auto bbox = getBoundingBox();
        float maxDim = std::max({bbox.max.x - bbox.min.x, bbox.max.y - bbox.min.y, bbox.max.z - bbox.min.z});
        cameraDistance_ = maxDim * 2.0f + 5.0f;
        cameraTarget_ = {(bbox.min.x + bbox.max.x) / 2, (bbox.min.y + bbox.max.y) / 2, (bbox.min.z + bbox.max.z) / 2};
    } else {
        cameraDistance_ = 15.0f;
    }
}

void TensorVisualizer::setCameraDistance(float distance) {
    cameraDistance_ = distance;
}

void TensorVisualizer::setCameraAngle(float yaw, float pitch) {
    cameraYaw_ = yaw;
    cameraPitch_ = pitch;
}

void TensorVisualizer::handleInput() {
    // Modus wechseln mit Tasten 1-7
    if (IsKeyPressed(KEY_ONE)) setMode(VisualizationMode::Cubes);
    if (IsKeyPressed(KEY_TWO)) setMode(VisualizationMode::Spheres);
    if (IsKeyPressed(KEY_THREE)) setMode(VisualizationMode::Points);
    if (IsKeyPressed(KEY_FOUR)) setMode(VisualizationMode::Heatmap);
    if (IsKeyPressed(KEY_FIVE)) setMode(VisualizationMode::Bars);
    if (IsKeyPressed(KEY_SIX)) setMode(VisualizationMode::Network);
    if (IsKeyPressed(KEY_SEVEN)) setMode(VisualizationMode::Memory);

    // Toggle-Optionen
    if (IsKeyPressed(KEY_G)) showGrid_ = !showGrid_;
    if (IsKeyPressed(KEY_X)) showAxes_ = !showAxes_;
    if (IsKeyPressed(KEY_V)) showValues_ = !showValues_;
    if (IsKeyPressed(KEY_C)) colorByValue_ = !colorByValue_;
    if (IsKeyPressed(KEY_R)) resetCamera();
}

void TensorVisualizer::animateEntry() {
    entryAnimation_ = 0.0f;
    for (auto& anim : elementAnimations_) {
        anim = 0.0f;
    }
}

void TensorVisualizer::animateValueChange(size_t index, float newValue) {
    if (index < elementAnimations_.size()) {
        elementAnimations_[index] = 0.0f;
    }
}

void TensorVisualizer::highlightElement(const std::vector<size_t>& indices) {
    highlightedIndices_ = indices;
    highlightPulse_ = 0.0f;
}

void TensorVisualizer::highlightSlice(size_t axis, size_t index) {
    highlightedIndices_.clear();
    if (!tensor_) return;

    // Alle Elemente mit dem gegebenen Index auf der Achse highlighten
    for (size_t i = 0; i < tensor_->size(); ++i) {
        // Berechne Indizes aus flachem Index (vereinfacht)
        auto indices = std::vector<size_t>();
        size_t remaining = i;
        for (size_t d = 0; d < tensor_->rank(); ++d) {
            size_t stride = tensor_->strides()[d];
            indices.push_back(remaining / stride);
            remaining %= stride;
        }

        if (axis < indices.size() && indices[axis] == index) {
            highlightedIndices_.insert(highlightedIndices_.end(), indices.begin(), indices.end());
        }
    }
}

BoundingBox TensorVisualizer::getBoundingBox() const {
    if (!tensor_ || tensor_->empty()) {
        return {{-1, -1, -1}, {1, 1, 1}};
    }

    Vector3 min = {0, 0, 0};
    Vector3 max = {0, 0, 0};

    const auto& shape = tensor_->shape();
    if (shape.size() >= 1) max.x = (shape[0] - 1) * spacing_;
    if (shape.size() >= 2) max.y = (shape[1] - 1) * spacing_;
    if (shape.size() >= 3) max.z = (shape[2] - 1) * spacing_;

    // Zentrieren
    Vector3 center = {(max.x - min.x) / 2, (max.y - min.y) / 2, (max.z - min.z) / 2};
    min.x -= center.x; max.x -= center.x;
    min.y -= center.y; max.y -= center.y;
    min.z -= center.z; max.z -= center.z;

    return {min, max};
}

void TensorVisualizer::drawCubesMode() {
    if (!tensor_) return;

    const auto& shape = tensor_->shape();
    Vector3 offset = {0, 0, 0};

    // Zentrieren
    if (shape.size() >= 1) offset.x = -(shape[0] - 1) * spacing_ / 2;
    if (shape.size() >= 2) offset.y = -(shape[1] - 1) * spacing_ / 2;
    if (shape.size() >= 3) offset.z = -(shape[2] - 1) * spacing_ / 2;

    for (size_t i = 0; i < tensor_->size(); ++i) {
        // Indizes berechnen
        std::vector<size_t> indices;
        size_t remaining = i;
        for (size_t d = 0; d < tensor_->rank(); ++d) {
            size_t stride = tensor_->strides()[d];
            indices.push_back(remaining / stride);
            remaining %= stride;
        }

        Vector3 pos = getElementPosition(indices);
        pos.x += offset.x;
        pos.y += offset.y;
        pos.z += offset.z;

        // Animation
        float anim = std::min(1.0f, entryAnimation_ * 3.0f - static_cast<float>(i) / tensor_->size() * 2.0f);
        anim = std::max(0.0f, anim);
        float animScale = anim * anim * (3 - 2 * anim);  // Smoothstep

        // Highlight-Check
        bool highlighted = false;
        if (!highlightedIndices_.empty() && indices.size() <= highlightedIndices_.size()) {
            highlighted = true;
            for (size_t d = 0; d < indices.size(); ++d) {
                if (indices[d] != highlightedIndices_[d]) {
                    highlighted = false;
                    break;
                }
            }
        }

        float size = elementSize_ * animScale;
        if (highlighted) {
            size *= 1.0f + 0.2f * sinf(highlightPulse_ * 4.0f);
        }

        Color color = getElementColor(normalizedValues_[i], highlighted);

        // Würfel zeichnen
        if (size > 0.01f) {
            DrawCube(pos, size, size, size, color);
            DrawCubeWires(pos, size * 1.01f, size * 1.01f, size * 1.01f,
                          {static_cast<unsigned char>(color.r / 2), static_cast<unsigned char>(color.g / 2), static_cast<unsigned char>(color.b / 2), 255});
        }

        // Wert anzeigen
        if (showValues_ && tensor_->size() <= 64 && size > 0.3f) {
            // Billboard-Text wäre besser, hier vereinfacht
        }
    }
}

void TensorVisualizer::drawSpheresMode() {
    if (!tensor_) return;

    const auto& shape = tensor_->shape();
    Vector3 offset = {0, 0, 0};

    if (shape.size() >= 1) offset.x = -(shape[0] - 1) * spacing_ / 2;
    if (shape.size() >= 2) offset.y = -(shape[1] - 1) * spacing_ / 2;
    if (shape.size() >= 3) offset.z = -(shape[2] - 1) * spacing_ / 2;

    for (size_t i = 0; i < tensor_->size(); ++i) {
        std::vector<size_t> indices;
        size_t remaining = i;
        for (size_t d = 0; d < tensor_->rank(); ++d) {
            size_t stride = tensor_->strides()[d];
            indices.push_back(remaining / stride);
            remaining %= stride;
        }

        Vector3 pos = getElementPosition(indices);
        pos.x += offset.x;
        pos.y += offset.y;
        pos.z += offset.z;

        float anim = std::min(1.0f, entryAnimation_ * 3.0f - static_cast<float>(i) / tensor_->size() * 2.0f);
        anim = std::max(0.0f, anim);
        float animScale = anim * anim * (3 - 2 * anim);

        float radius = (elementSize_ / 2) * animScale;
        // Größe basierend auf Wert
        radius *= 0.5f + normalizedValues_[i] * 0.5f;

        Color color = getElementColor(normalizedValues_[i], false);

        if (radius > 0.01f) {
            DrawSphere(pos, radius, color);
        }
    }
}

void TensorVisualizer::drawPointsMode() {
    if (!tensor_) return;

    const auto& shape = tensor_->shape();
    Vector3 offset = {0, 0, 0};

    if (shape.size() >= 1) offset.x = -(shape[0] - 1) * spacing_ / 2;
    if (shape.size() >= 2) offset.y = -(shape[1] - 1) * spacing_ / 2;
    if (shape.size() >= 3) offset.z = -(shape[2] - 1) * spacing_ / 2;

    for (size_t i = 0; i < tensor_->size(); ++i) {
        std::vector<size_t> indices;
        size_t remaining = i;
        for (size_t d = 0; d < tensor_->rank(); ++d) {
            size_t stride = tensor_->strides()[d];
            indices.push_back(remaining / stride);
            remaining %= stride;
        }

        Vector3 pos = getElementPosition(indices);
        pos.x += offset.x;
        pos.y += offset.y;
        pos.z += offset.z;

        Color color = getElementColor(normalizedValues_[i], false);
        DrawPoint3D(pos, color);

        // Größere Punkte simulieren
        DrawSphere(pos, 0.05f, color);
    }
}

void TensorVisualizer::drawHeatmapMode() {
    if (!tensor_ || tensor_->rank() != 2) return;

    const auto& shape = tensor_->shape();
    float cellSize = elementSize_;

    Vector3 offset = {-(shape[0] - 1) * cellSize / 2, 0, -(shape[1] - 1) * cellSize / 2};

    for (size_t i = 0; i < shape[0]; ++i) {
        for (size_t j = 0; j < shape[1]; ++j) {
            float value = tensor_->at(i, j);
            size_t idx = i * shape[1] + j;
            Color color = getElementColor(normalizedValues_[idx], false);

            Vector3 pos = {
                offset.x + i * cellSize,
                value * 2.0f,  // Höhe basierend auf Wert
                offset.z + j * cellSize
            };

            float anim = std::min(1.0f, entryAnimation_ * 2.0f);
            float height = 0.1f + normalizedValues_[idx] * 2.0f * anim;

            DrawCube(pos, cellSize * 0.95f, height, cellSize * 0.95f, color);
        }
    }
}

void TensorVisualizer::drawBarsMode() {
    if (!tensor_ || tensor_->rank() != 1) return;

    size_t n = tensor_->shape()[0];
    float barWidth = elementSize_;
    float maxHeight = 5.0f;

    Vector3 offset = {-(n - 1) * barWidth * 1.2f / 2, 0, 0};

    for (size_t i = 0; i < n; ++i) {
        float value = (*tensor_)[i];
        Color color = getElementColor(normalizedValues_[i], false);

        float anim = std::min(1.0f, entryAnimation_ * 3.0f - static_cast<float>(i) / n * 2.0f);
        anim = std::max(0.0f, anim);
        float height = normalizedValues_[i] * maxHeight * anim;

        Vector3 pos = {offset.x + i * barWidth * 1.2f, height / 2, 0};

        DrawCube(pos, barWidth, height, barWidth, color);
        DrawCubeWires(pos, barWidth * 1.01f, height * 1.01f, barWidth * 1.01f, Colors::BORDER);
    }
}

void TensorVisualizer::drawNetworkMode() {
    if (!tensor_) return;

    // Verbindungen zwischen benachbarten Elementen zeichnen
    const auto& shape = tensor_->shape();
    Vector3 offset = {0, 0, 0};

    if (shape.size() >= 1) offset.x = -(shape[0] - 1) * spacing_ / 2;
    if (shape.size() >= 2) offset.y = -(shape[1] - 1) * spacing_ / 2;
    if (shape.size() >= 3) offset.z = -(shape[2] - 1) * spacing_ / 2;

    // Erst Linien
    for (size_t i = 0; i < tensor_->size(); ++i) {
        std::vector<size_t> indices;
        size_t remaining = i;
        for (size_t d = 0; d < tensor_->rank(); ++d) {
            size_t stride = tensor_->strides()[d];
            indices.push_back(remaining / stride);
            remaining %= stride;
        }

        Vector3 pos = getElementPosition(indices);
        pos.x += offset.x;
        pos.y += offset.y;
        pos.z += offset.z;

        // Verbindungen zu Nachbarn
        for (size_t d = 0; d < indices.size(); ++d) {
            if (indices[d] + 1 < shape[d]) {
                std::vector<size_t> neighborIdx = indices;
                neighborIdx[d]++;
                Vector3 neighborPos = getElementPosition(neighborIdx);
                neighborPos.x += offset.x;
                neighborPos.y += offset.y;
                neighborPos.z += offset.z;

                DrawLine3D(pos, neighborPos, {100, 100, 150, 100});
            }
        }
    }

    // Dann Knoten
    for (size_t i = 0; i < tensor_->size(); ++i) {
        std::vector<size_t> indices;
        size_t remaining = i;
        for (size_t d = 0; d < tensor_->rank(); ++d) {
            size_t stride = tensor_->strides()[d];
            indices.push_back(remaining / stride);
            remaining %= stride;
        }

        Vector3 pos = getElementPosition(indices);
        pos.x += offset.x;
        pos.y += offset.y;
        pos.z += offset.z;

        float anim = std::min(1.0f, entryAnimation_ * 2.0f);
        Color color = getElementColor(normalizedValues_[i], false);

        DrawSphere(pos, elementSize_ * 0.3f * anim, color);
    }
}

void TensorVisualizer::drawMemoryMode() {
    if (!tensor_) return;

    // Zeige Speicher-Layout als 1D-Streifen
    size_t n = tensor_->size();
    float cellWidth = 1.0f;
    float cellHeight = 0.5f;

    // In Zeilen aufteilen
    size_t cols = std::min(n, static_cast<size_t>(16));
    size_t rows = (n + cols - 1) / cols;

    Vector3 offset = {-(cols - 1) * cellWidth / 2, (rows - 1) * cellHeight / 2, 0};

    for (size_t i = 0; i < n; ++i) {
        size_t row = i / cols;
        size_t col = i % cols;

        float anim = std::min(1.0f, entryAnimation_ * 3.0f - static_cast<float>(i) / n * 2.0f);
        anim = std::max(0.0f, anim);

        Vector3 pos = {offset.x + col * cellWidth, offset.y - row * cellHeight * 1.2f, 0};

        Color color = getElementColor(normalizedValues_[i], false);
        color.a = static_cast<unsigned char>(255 * anim);

        DrawCube(pos, cellWidth * 0.9f * anim, cellHeight * 0.9f * anim, 0.2f, color);
        DrawCubeWires(pos, cellWidth * 0.91f * anim, cellHeight * 0.91f * anim, 0.21f, Colors::BORDER);
    }

    // Adress-Labels
    DrawText("Memory Address: 0x0000", 10, GetScreenHeight() - 60, 14, Colors::TEXT_DIM);
}

void TensorVisualizer::drawGrid() {
    int slices = 20;
    float spacing = 1.0f;

    rlPushMatrix();
    rlRotatef(90.0f, 1.0f, 0.0f, 0.0f);

    rlBegin(RL_LINES);
    for (int i = -slices; i <= slices; i++) {
        if (i == 0) {
            rlColor4ub(Colors::GRID.r, Colors::GRID.g, Colors::GRID.b, Colors::GRID.a * 2);
        } else {
            rlColor4ub(Colors::GRID.r, Colors::GRID.g, Colors::GRID.b, Colors::GRID.a);
        }

        rlVertex3f(static_cast<float>(i) * spacing, -slices * spacing, 0.0f);
        rlVertex3f(static_cast<float>(i) * spacing, slices * spacing, 0.0f);

        rlVertex3f(-slices * spacing, static_cast<float>(i) * spacing, 0.0f);
        rlVertex3f(slices * spacing, static_cast<float>(i) * spacing, 0.0f);
    }
    rlEnd();

    rlPopMatrix();
}

void TensorVisualizer::drawAxes() {
    float length = 3.0f;
    float thickness = 0.05f;

    // X-Achse (Rot)
    DrawCylinder({0, 0, 0}, thickness, thickness, length, 8, Colors::GRID_AXIS_X);
    DrawCylinder({length, 0, 0}, thickness * 2, 0, 0.3f, 8, Colors::GRID_AXIS_X);

    // Y-Achse (Grün)
    rlPushMatrix();
    rlRotatef(-90, 0, 0, 1);
    DrawCylinder({0, 0, 0}, thickness, thickness, length, 8, Colors::GRID_AXIS_Y);
    DrawCylinder({length, 0, 0}, thickness * 2, 0, 0.3f, 8, Colors::GRID_AXIS_Y);
    rlPopMatrix();

    // Z-Achse (Blau)
    rlPushMatrix();
    rlRotatef(90, 0, 1, 0);
    DrawCylinder({0, 0, 0}, thickness, thickness, length, 8, Colors::GRID_AXIS_Z);
    DrawCylinder({length, 0, 0}, thickness * 2, 0, 0.3f, 8, Colors::GRID_AXIS_Z);
    rlPopMatrix();

    // Labels
    // (würde 3D-Text erfordern, hier vereinfacht weggelassen)
}

Vector3 TensorVisualizer::getElementPosition(const std::vector<size_t>& indices) const {
    Vector3 pos = {0, 0, 0};
    if (indices.size() >= 1) pos.x = indices[0] * spacing_;
    if (indices.size() >= 2) pos.y = indices[1] * spacing_;
    if (indices.size() >= 3) pos.z = indices[2] * spacing_;
    return pos;
}

Color TensorVisualizer::getElementColor(float normalizedValue, bool highlighted) const {
    Color base = colorByValue_ ? Colors::valueToColor(normalizedValue) : Colors::PRIMARY;

    if (highlighted) {
        float pulse = 0.5f + 0.5f * sinf(highlightPulse_ * 6.0f);
        base.r = static_cast<unsigned char>(std::min(255.0f, base.r + 100 * pulse));
        base.g = static_cast<unsigned char>(std::min(255.0f, base.g + 100 * pulse));
        base.b = static_cast<unsigned char>(std::min(255.0f, base.b + 100 * pulse));
    }

    return base;
}

void TensorVisualizer::updateSelection() {
    if (!tensor_) {
        selected_.valid = false;
        return;
    }

    // Ray-Casting für Auswahl
    Vector2 mouse = GetMousePosition();
    mouseRay_ = GetScreenToWorldRay(mouse, camera_);

    selected_.valid = false;
    float closestDist = 1000.0f;

    const auto& shape = tensor_->shape();
    Vector3 offset = {0, 0, 0};

    if (shape.size() >= 1) offset.x = -(shape[0] - 1) * spacing_ / 2;
    if (shape.size() >= 2) offset.y = -(shape[1] - 1) * spacing_ / 2;
    if (shape.size() >= 3) offset.z = -(shape[2] - 1) * spacing_ / 2;

    for (size_t i = 0; i < tensor_->size(); ++i) {
        std::vector<size_t> indices;
        size_t remaining = i;
        for (size_t d = 0; d < tensor_->rank(); ++d) {
            size_t stride = tensor_->strides()[d];
            indices.push_back(remaining / stride);
            remaining %= stride;
        }

        Vector3 pos = getElementPosition(indices);
        pos.x += offset.x;
        pos.y += offset.y;
        pos.z += offset.z;

        BoundingBox box = {
            {pos.x - elementSize_ / 2, pos.y - elementSize_ / 2, pos.z - elementSize_ / 2},
            {pos.x + elementSize_ / 2, pos.y + elementSize_ / 2, pos.z + elementSize_ / 2}
        };

        RayCollision collision = GetRayCollisionBox(mouseRay_, box);
        if (collision.hit && collision.distance < closestDist) {
            closestDist = collision.distance;
            selected_.indices = indices;
            selected_.value = (*tensor_)[i];
            selected_.worldPosition = pos;
            selected_.valid = true;
        }
    }
}

void TensorVisualizer::updateAnimations(float deltaTime) {
    // Eingangsanimation
    if (entryAnimation_ < 1.0f) {
        entryAnimation_ += deltaTime * 1.5f;
        if (entryAnimation_ > 1.0f) entryAnimation_ = 1.0f;
    }

    // Highlight-Puls
    highlightPulse_ += deltaTime;

    // Element-Animationen
    for (auto& anim : elementAnimations_) {
        if (anim < 1.0f) {
            anim += deltaTime * 3.0f;
            if (anim > 1.0f) anim = 1.0f;
        }
    }
}

// === TensorCompareVisualizer ===

void TensorCompareVisualizer::setTensors(const tensor::Tensor& a, const tensor::Tensor& b) {
    vizA_.setTensor(a, "Tensor A");
    vizB_.setTensor(b, "Tensor B");
}

void TensorCompareVisualizer::render3D() {
    // Zwei Tensoren nebeneinander
    // (vereinfachte Implementierung)
    vizA_.render3D();
}

void TensorCompareVisualizer::renderOverlay() {
    vizA_.renderOverlay();
}

// === OperationVisualizer ===

void OperationVisualizer::setOperation(Operation op, const tensor::Tensor& a, const tensor::Tensor& b) {
    operation_ = op;
    tensorA_ = a;
    tensorB_ = b;
    progress_ = 0.0f;
    playing_ = false;

    // Ergebnis berechnen
    switch (op) {
        case Operation::Addition:
            result_ = a + b;
            break;
        case Operation::Subtraction:
            result_ = a - b;
            break;
        case Operation::Multiplication:
            result_ = a * b;
            break;
        case Operation::Division:
            result_ = a / b;
            break;
        case Operation::MatMul:
            result_ = a.matmul(b);
            break;
        default:
            result_ = a;
            break;
    }

    vizA_.setTensor(tensorA_, "A");
    vizB_.setTensor(tensorB_, "B");
    vizResult_.setTensor(result_, "Result");
}

void OperationVisualizer::setOperation(Operation op, const tensor::Tensor& input) {
    operation_ = op;
    tensorA_ = input;
    progress_ = 0.0f;
    playing_ = false;

    switch (op) {
        case Operation::Transpose:
            result_ = input.transpose();
            break;
        case Operation::Reshape:
            // Würde zusätzliche Parameter benötigen
            result_ = input;
            break;
        default:
            result_ = input;
            break;
    }

    vizA_.setTensor(tensorA_, "Input");
    vizResult_.setTensor(result_, "Result");
}

void OperationVisualizer::play() {
    playing_ = true;
}

void OperationVisualizer::pause() {
    playing_ = false;
}

void OperationVisualizer::reset() {
    progress_ = 0.0f;
    playing_ = false;
}

void OperationVisualizer::update(float deltaTime) {
    if (playing_ && progress_ < 1.0f) {
        progress_ += deltaTime * speed_ * 0.3f;
        if (progress_ > 1.0f) progress_ = 1.0f;
    }

    vizA_.update(deltaTime);
    vizB_.update(deltaTime);
    vizResult_.update(deltaTime);
}

void OperationVisualizer::render3D() {
    // Vereinfachte Darstellung
    vizA_.render3D();
}

void OperationVisualizer::renderOverlay() {
    vizA_.renderOverlay();

    // Fortschrittsbalken
    Rectangle progressBounds = {
        static_cast<float>(GetScreenWidth()) / 2 - 200,
        static_cast<float>(GetScreenHeight()) - 60,
        400, 20
    };

    DrawRectangleRounded(progressBounds, 0.5f, 8, Colors::BACKGROUND_LIGHT);
    Rectangle fill = {progressBounds.x, progressBounds.y, progressBounds.width * progress_, progressBounds.height};
    DrawRectangleRounded(fill, 0.5f, 8, Colors::PRIMARY);
}

} // namespace gui
