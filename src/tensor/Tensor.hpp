#pragma once

#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <stdexcept>
#include <cmath>
#include <numeric>
#include <sstream>
#include <iomanip>

namespace tensor {

/**
 * @brief Multidimensionale Tensor-Klasse
 *
 * Ein Tensor ist eine Verallgemeinerung von Skalaren, Vektoren und Matrizen
 * auf beliebig viele Dimensionen.
 *
 * - Rang 0: Skalar (einzelner Wert)
 * - Rang 1: Vektor (1D Array)
 * - Rang 2: Matrix (2D Array)
 * - Rang 3+: Höherdimensionaler Tensor
 */
class Tensor {
public:
    using Shape = std::vector<size_t>;
    using DataType = float;

    // === Konstruktoren ===

    // Leerer Tensor
    Tensor();

    // Skalar
    explicit Tensor(DataType value);

    // Tensor mit gegebener Form, initialisiert mit 0
    explicit Tensor(const Shape& shape);

    // Tensor mit gegebener Form und Daten
    Tensor(const Shape& shape, const std::vector<DataType>& data);

    // Tensor mit gegebener Form und Initializer
    Tensor(const Shape& shape, std::function<DataType(size_t)> initializer);

    // Copy & Move
    Tensor(const Tensor& other) = default;
    Tensor(Tensor&& other) noexcept = default;
    Tensor& operator=(const Tensor& other) = default;
    Tensor& operator=(Tensor&& other) noexcept = default;

    // === Fabrikmethoden ===

    static Tensor zeros(const Shape& shape);
    static Tensor ones(const Shape& shape);
    static Tensor fill(const Shape& shape, DataType value);
    static Tensor random(const Shape& shape, DataType min = 0.0f, DataType max = 1.0f);
    static Tensor range(DataType start, DataType end, DataType step = 1.0f);
    static Tensor identity(size_t n);
    static Tensor fromVector(const std::vector<DataType>& vec);
    static Tensor fromMatrix(const std::vector<std::vector<DataType>>& mat);

    // === Eigenschaften ===

    const Shape& shape() const { return shape_; }
    size_t rank() const { return shape_.size(); }
    size_t size() const { return data_.size(); }
    bool empty() const { return data_.empty(); }

    // Dimensionsgröße
    size_t dim(size_t axis) const;

    // Strides für Indexberechnung
    const std::vector<size_t>& strides() const { return strides_; }

    // === Datenzugriff ===

    // Linearer Zugriff
    DataType& operator[](size_t index);
    const DataType& operator[](size_t index) const;

    // Multidimensionaler Zugriff
    DataType& at(const std::vector<size_t>& indices);
    const DataType& at(const std::vector<size_t>& indices) const;

    // Für 2D (Matrix)
    DataType& at(size_t row, size_t col);
    const DataType& at(size_t row, size_t col) const;

    // Für 3D
    DataType& at(size_t i, size_t j, size_t k);
    const DataType& at(size_t i, size_t j, size_t k) const;

    // Rohdaten
    std::vector<DataType>& data() { return data_; }
    const std::vector<DataType>& data() const { return data_; }

    // === Umformung ===

    Tensor reshape(const Shape& newShape) const;
    Tensor flatten() const;
    Tensor transpose() const;  // Für 2D Matrizen
    Tensor transpose(const std::vector<size_t>& axes) const;  // Allgemein
    Tensor squeeze() const;  // Entfernt Dimensionen der Größe 1
    Tensor unsqueeze(size_t axis) const;  // Fügt Dimension hinzu

    // === Slicing ===

    Tensor slice(size_t axis, size_t start, size_t end) const;
    Tensor row(size_t i) const;  // Für Matrizen
    Tensor col(size_t j) const;  // Für Matrizen

    // === Elementweise Operationen ===

    Tensor operator+(const Tensor& other) const;
    Tensor operator-(const Tensor& other) const;
    Tensor operator*(const Tensor& other) const;  // Elementweise!
    Tensor operator/(const Tensor& other) const;

    Tensor operator+(DataType scalar) const;
    Tensor operator-(DataType scalar) const;
    Tensor operator*(DataType scalar) const;
    Tensor operator/(DataType scalar) const;

    Tensor& operator+=(const Tensor& other);
    Tensor& operator-=(const Tensor& other);
    Tensor& operator*=(const Tensor& other);
    Tensor& operator/=(const Tensor& other);

    Tensor operator-() const;  // Negation

    // Elementweise Funktionen
    Tensor apply(std::function<DataType(DataType)> func) const;
    Tensor sqrt() const;
    Tensor pow(DataType exponent) const;
    Tensor exp() const;
    Tensor log() const;
    Tensor abs() const;
    Tensor sin() const;
    Tensor cos() const;

    // === Reduktionen ===

    DataType sum() const;
    DataType mean() const;
    DataType min() const;
    DataType max() const;
    DataType prod() const;

    // Achsenweise Reduktionen
    Tensor sum(size_t axis) const;
    Tensor mean(size_t axis) const;
    Tensor min(size_t axis) const;
    Tensor max(size_t axis) const;

    // === Matrixoperationen ===

    Tensor matmul(const Tensor& other) const;  // Matrixmultiplikation
    Tensor dot(const Tensor& other) const;     // Skalarprodukt für Vektoren
    DataType norm() const;                      // Euklidische Norm
    Tensor normalize() const;                   // Normalisieren

    // === Vergleiche ===

    bool operator==(const Tensor& other) const;
    bool operator!=(const Tensor& other) const;
    bool allClose(const Tensor& other, DataType rtol = 1e-5f, DataType atol = 1e-8f) const;

    // === String-Darstellung ===

    std::string toString() const;
    std::string toDetailedString() const;
    std::string shapeString() const;

    // === Hilfsmethoden für Visualisierung ===

    // Gibt Werte in normalisierten Bereich [0,1] zurück
    std::vector<DataType> normalizedData() const;

    // Für 3D-Visualisierung: Position im Raum für jeden Index
    struct Point3D { float x, y, z; };
    std::vector<Point3D> get3DPositions(float spacing = 1.0f) const;

private:
    Shape shape_;
    std::vector<size_t> strides_;
    std::vector<DataType> data_;

    void computeStrides();
    size_t flatIndex(const std::vector<size_t>& indices) const;
    std::vector<size_t> unflatIndex(size_t flatIdx) const;
    void validateShape(const Shape& shape) const;
    void validateIndices(const std::vector<size_t>& indices) const;
    void broadcastShapes(const Tensor& other, Shape& resultShape) const;
};

// Freie Funktionen für Operationen
Tensor matmul(const Tensor& a, const Tensor& b);
Tensor concatenate(const std::vector<Tensor>& tensors, size_t axis);
Tensor stack(const std::vector<Tensor>& tensors, size_t axis = 0);

} // namespace tensor
