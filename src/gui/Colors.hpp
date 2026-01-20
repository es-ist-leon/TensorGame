#pragma once

#include "raylib.h"

namespace gui {

/**
 * @brief Moderne Farbpalette für die Tensor-Visualisierung
 */
namespace Colors {
    // Hintergrund
    constexpr Color BACKGROUND = {15, 15, 25, 255};
    constexpr Color BACKGROUND_LIGHT = {25, 25, 40, 255};
    constexpr Color BACKGROUND_PANEL = {30, 30, 50, 255};

    // Primärfarben
    constexpr Color PRIMARY = {100, 149, 237, 255};      // Cornflower Blue
    constexpr Color PRIMARY_DARK = {65, 105, 225, 255};  // Royal Blue
    constexpr Color PRIMARY_LIGHT = {135, 206, 250, 255}; // Light Sky Blue

    // Akzentfarben
    constexpr Color ACCENT = {255, 107, 107, 255};       // Coral Red
    constexpr Color ACCENT_GREEN = {80, 200, 120, 255};  // Emerald
    constexpr Color ACCENT_YELLOW = {255, 215, 0, 255};  // Gold
    constexpr Color ACCENT_PURPLE = {147, 112, 219, 255}; // Medium Purple

    // Text
    constexpr Color TEXT = {240, 240, 250, 255};
    constexpr Color TEXT_DIM = {150, 150, 170, 255};
    constexpr Color TEXT_HIGHLIGHT = {255, 255, 255, 255};

    // UI Elemente
    constexpr Color BUTTON = {60, 60, 90, 255};
    constexpr Color BUTTON_HOVER = {80, 80, 120, 255};
    constexpr Color BUTTON_ACTIVE = {100, 100, 150, 255};
    constexpr Color BORDER = {70, 70, 100, 255};

    // Tensor-Visualisierung
    constexpr Color TENSOR_SCALAR = {255, 215, 0, 255};   // Gold für Skalare
    constexpr Color TENSOR_VECTOR = {100, 149, 237, 255}; // Blau für Vektoren
    constexpr Color TENSOR_MATRIX = {147, 112, 219, 255}; // Lila für Matrizen
    constexpr Color TENSOR_3D = {80, 200, 120, 255};      // Grün für 3D

    // Grid
    constexpr Color GRID = {40, 40, 60, 100};
    constexpr Color GRID_AXIS_X = {255, 100, 100, 200};
    constexpr Color GRID_AXIS_Y = {100, 255, 100, 200};
    constexpr Color GRID_AXIS_Z = {100, 100, 255, 200};

    // Wert-Farbverlauf (für Tensor-Werte)
    inline Color valueToColor(float normalizedValue) {
        // Blau (niedrig) -> Grün (mittel) -> Rot (hoch)
        unsigned char r, g, b;

        if (normalizedValue < 0.5f) {
            float t = normalizedValue * 2.0f;
            r = static_cast<unsigned char>(50 * (1 - t) + 80 * t);
            g = static_cast<unsigned char>(100 * (1 - t) + 200 * t);
            b = static_cast<unsigned char>(200 * (1 - t) + 120 * t);
        } else {
            float t = (normalizedValue - 0.5f) * 2.0f;
            r = static_cast<unsigned char>(80 * (1 - t) + 255 * t);
            g = static_cast<unsigned char>(200 * (1 - t) + 100 * t);
            b = static_cast<unsigned char>(120 * (1 - t) + 100 * t);
        }

        return {r, g, b, 255};
    }

    // Rang-Farbe
    inline Color rankColor(size_t rank) {
        switch (rank) {
            case 0: return TENSOR_SCALAR;
            case 1: return TENSOR_VECTOR;
            case 2: return TENSOR_MATRIX;
            default: return TENSOR_3D;
        }
    }
}

} // namespace gui
