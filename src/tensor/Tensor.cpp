#include "tensor/Tensor.hpp"
#include <algorithm>
#include <random>
#include <cassert>

namespace tensor {

// === Konstruktoren ===

Tensor::Tensor() : shape_(), strides_(), data_() {}

Tensor::Tensor(DataType value) : shape_(), strides_(), data_({value}) {}

Tensor::Tensor(const Shape& shape) : shape_(shape) {
    validateShape(shape);
    computeStrides();
    size_t totalSize = shape.empty() ? 1 : strides_[0] * shape[0];
    data_.resize(totalSize, 0.0f);
}

Tensor::Tensor(const Shape& shape, const std::vector<DataType>& data)
    : shape_(shape), data_(data) {
    validateShape(shape);
    computeStrides();
    size_t expectedSize = shape.empty() ? 1 : strides_[0] * shape[0];
    if (data.size() != expectedSize) {
        throw std::invalid_argument("Data size doesn't match shape");
    }
}

Tensor::Tensor(const Shape& shape, std::function<DataType(size_t)> initializer)
    : shape_(shape) {
    validateShape(shape);
    computeStrides();
    size_t totalSize = shape.empty() ? 1 : strides_[0] * shape[0];
    data_.resize(totalSize);
    for (size_t i = 0; i < totalSize; ++i) {
        data_[i] = initializer(i);
    }
}

// === Fabrikmethoden ===

Tensor Tensor::zeros(const Shape& shape) {
    return Tensor(shape);
}

Tensor Tensor::ones(const Shape& shape) {
    return fill(shape, 1.0f);
}

Tensor Tensor::fill(const Shape& shape, DataType value) {
    Tensor t(shape);
    std::fill(t.data_.begin(), t.data_.end(), value);
    return t;
}

Tensor Tensor::random(const Shape& shape, DataType min, DataType max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<DataType> dist(min, max);

    return Tensor(shape, [&](size_t) { return dist(gen); });
}

Tensor Tensor::range(DataType start, DataType end, DataType step) {
    std::vector<DataType> data;
    for (DataType v = start; v < end; v += step) {
        data.push_back(v);
    }
    return Tensor({data.size()}, data);
}

Tensor Tensor::identity(size_t n) {
    Tensor t({n, n});
    for (size_t i = 0; i < n; ++i) {
        t.at(i, i) = 1.0f;
    }
    return t;
}

Tensor Tensor::fromVector(const std::vector<DataType>& vec) {
    return Tensor({vec.size()}, vec);
}

Tensor Tensor::fromMatrix(const std::vector<std::vector<DataType>>& mat) {
    if (mat.empty()) return Tensor({0, 0});
    size_t rows = mat.size();
    size_t cols = mat[0].size();
    std::vector<DataType> data;
    data.reserve(rows * cols);
    for (const auto& row : mat) {
        if (row.size() != cols) {
            throw std::invalid_argument("Inconsistent row sizes");
        }
        data.insert(data.end(), row.begin(), row.end());
    }
    return Tensor({rows, cols}, data);
}

// === Eigenschaften ===

size_t Tensor::dim(size_t axis) const {
    if (axis >= shape_.size()) {
        throw std::out_of_range("Axis out of range");
    }
    return shape_[axis];
}

// === Datenzugriff ===

Tensor::DataType& Tensor::operator[](size_t index) {
    return data_[index];
}

const Tensor::DataType& Tensor::operator[](size_t index) const {
    return data_[index];
}

Tensor::DataType& Tensor::at(const std::vector<size_t>& indices) {
    validateIndices(indices);
    return data_[flatIndex(indices)];
}

const Tensor::DataType& Tensor::at(const std::vector<size_t>& indices) const {
    validateIndices(indices);
    return data_[flatIndex(indices)];
}

Tensor::DataType& Tensor::at(size_t row, size_t col) {
    return at({row, col});
}

const Tensor::DataType& Tensor::at(size_t row, size_t col) const {
    return at({row, col});
}

Tensor::DataType& Tensor::at(size_t i, size_t j, size_t k) {
    return at({i, j, k});
}

const Tensor::DataType& Tensor::at(size_t i, size_t j, size_t k) const {
    return at({i, j, k});
}

// === Umformung ===

Tensor Tensor::reshape(const Shape& newShape) const {
    size_t newSize = 1;
    for (size_t d : newShape) newSize *= d;
    if (newSize != data_.size()) {
        throw std::invalid_argument("Cannot reshape: incompatible sizes");
    }
    return Tensor(newShape, data_);
}

Tensor Tensor::flatten() const {
    return Tensor({data_.size()}, data_);
}

Tensor Tensor::transpose() const {
    if (rank() != 2) {
        throw std::invalid_argument("transpose() without args only for 2D tensors");
    }
    Tensor result({shape_[1], shape_[0]});
    for (size_t i = 0; i < shape_[0]; ++i) {
        for (size_t j = 0; j < shape_[1]; ++j) {
            result.at(j, i) = at(i, j);
        }
    }
    return result;
}

Tensor Tensor::transpose(const std::vector<size_t>& axes) const {
    if (axes.size() != rank()) {
        throw std::invalid_argument("Axes must match tensor rank");
    }
    Shape newShape(rank());
    for (size_t i = 0; i < rank(); ++i) {
        newShape[i] = shape_[axes[i]];
    }
    Tensor result(newShape);
    for (size_t i = 0; i < data_.size(); ++i) {
        auto oldIdx = unflatIndex(i);
        std::vector<size_t> newIdx(rank());
        for (size_t j = 0; j < rank(); ++j) {
            newIdx[j] = oldIdx[axes[j]];
        }
        result.at(newIdx) = data_[i];
    }
    return result;
}

Tensor Tensor::squeeze() const {
    Shape newShape;
    for (size_t d : shape_) {
        if (d != 1) newShape.push_back(d);
    }
    if (newShape.empty()) newShape.push_back(1);
    return reshape(newShape);
}

Tensor Tensor::unsqueeze(size_t axis) const {
    if (axis > rank()) {
        throw std::out_of_range("Axis out of range for unsqueeze");
    }
    Shape newShape = shape_;
    newShape.insert(newShape.begin() + axis, 1);
    return reshape(newShape);
}

// === Slicing ===

Tensor Tensor::slice(size_t axis, size_t start, size_t end) const {
    if (axis >= rank()) throw std::out_of_range("Axis out of range");
    if (start >= end || end > shape_[axis]) {
        throw std::out_of_range("Invalid slice range");
    }

    Shape newShape = shape_;
    newShape[axis] = end - start;
    Tensor result(newShape);

    // Iteriere durch alle Indizes
    for (size_t i = 0; i < result.size(); ++i) {
        auto idx = result.unflatIndex(i);
        idx[axis] += start;
        result[i] = at(idx);
    }
    return result;
}

Tensor Tensor::row(size_t i) const {
    if (rank() != 2) throw std::invalid_argument("row() only for 2D tensors");
    return slice(0, i, i + 1).squeeze();
}

Tensor Tensor::col(size_t j) const {
    if (rank() != 2) throw std::invalid_argument("col() only for 2D tensors");
    return slice(1, j, j + 1).squeeze();
}

// === Elementweise Operationen ===

Tensor Tensor::operator+(const Tensor& other) const {
    if (shape_ != other.shape_) {
        throw std::invalid_argument("Shape mismatch for addition");
    }
    Tensor result(shape_);
    for (size_t i = 0; i < data_.size(); ++i) {
        result[i] = data_[i] + other[i];
    }
    return result;
}

Tensor Tensor::operator-(const Tensor& other) const {
    if (shape_ != other.shape_) {
        throw std::invalid_argument("Shape mismatch for subtraction");
    }
    Tensor result(shape_);
    for (size_t i = 0; i < data_.size(); ++i) {
        result[i] = data_[i] - other[i];
    }
    return result;
}

Tensor Tensor::operator*(const Tensor& other) const {
    if (shape_ != other.shape_) {
        throw std::invalid_argument("Shape mismatch for multiplication");
    }
    Tensor result(shape_);
    for (size_t i = 0; i < data_.size(); ++i) {
        result[i] = data_[i] * other[i];
    }
    return result;
}

Tensor Tensor::operator/(const Tensor& other) const {
    if (shape_ != other.shape_) {
        throw std::invalid_argument("Shape mismatch for division");
    }
    Tensor result(shape_);
    for (size_t i = 0; i < data_.size(); ++i) {
        result[i] = data_[i] / other[i];
    }
    return result;
}

Tensor Tensor::operator+(DataType scalar) const {
    return apply([scalar](DataType x) { return x + scalar; });
}

Tensor Tensor::operator-(DataType scalar) const {
    return apply([scalar](DataType x) { return x - scalar; });
}

Tensor Tensor::operator*(DataType scalar) const {
    return apply([scalar](DataType x) { return x * scalar; });
}

Tensor Tensor::operator/(DataType scalar) const {
    return apply([scalar](DataType x) { return x / scalar; });
}

Tensor& Tensor::operator+=(const Tensor& other) {
    *this = *this + other;
    return *this;
}

Tensor& Tensor::operator-=(const Tensor& other) {
    *this = *this - other;
    return *this;
}

Tensor& Tensor::operator*=(const Tensor& other) {
    *this = *this * other;
    return *this;
}

Tensor& Tensor::operator/=(const Tensor& other) {
    *this = *this / other;
    return *this;
}

Tensor Tensor::operator-() const {
    return apply([](DataType x) { return -x; });
}

// === Elementweise Funktionen ===

Tensor Tensor::apply(std::function<DataType(DataType)> func) const {
    Tensor result(shape_);
    for (size_t i = 0; i < data_.size(); ++i) {
        result[i] = func(data_[i]);
    }
    return result;
}

Tensor Tensor::sqrt() const {
    return apply([](DataType x) { return std::sqrt(x); });
}

Tensor Tensor::pow(DataType exponent) const {
    return apply([exponent](DataType x) { return std::pow(x, exponent); });
}

Tensor Tensor::exp() const {
    return apply([](DataType x) { return std::exp(x); });
}

Tensor Tensor::log() const {
    return apply([](DataType x) { return std::log(x); });
}

Tensor Tensor::abs() const {
    return apply([](DataType x) { return std::abs(x); });
}

Tensor Tensor::sin() const {
    return apply([](DataType x) { return std::sin(x); });
}

Tensor Tensor::cos() const {
    return apply([](DataType x) { return std::cos(x); });
}

// === Reduktionen ===

Tensor::DataType Tensor::sum() const {
    return std::accumulate(data_.begin(), data_.end(), DataType(0));
}

Tensor::DataType Tensor::mean() const {
    return sum() / static_cast<DataType>(data_.size());
}

Tensor::DataType Tensor::min() const {
    return *std::min_element(data_.begin(), data_.end());
}

Tensor::DataType Tensor::max() const {
    return *std::max_element(data_.begin(), data_.end());
}

Tensor::DataType Tensor::prod() const {
    return std::accumulate(data_.begin(), data_.end(), DataType(1),
                           std::multiplies<DataType>());
}

Tensor Tensor::sum(size_t axis) const {
    if (axis >= rank()) throw std::out_of_range("Axis out of range");

    Shape newShape;
    for (size_t i = 0; i < rank(); ++i) {
        if (i != axis) newShape.push_back(shape_[i]);
    }
    if (newShape.empty()) newShape.push_back(1);

    Tensor result = Tensor::zeros(newShape);
    for (size_t i = 0; i < data_.size(); ++i) {
        auto idx = unflatIndex(i);
        std::vector<size_t> newIdx;
        for (size_t j = 0; j < idx.size(); ++j) {
            if (j != axis) newIdx.push_back(idx[j]);
        }
        if (newIdx.empty()) newIdx.push_back(0);
        result.at(newIdx) += data_[i];
    }
    return result;
}

Tensor Tensor::mean(size_t axis) const {
    return sum(axis) / static_cast<DataType>(shape_[axis]);
}

Tensor Tensor::min(size_t axis) const {
    if (axis >= rank()) throw std::out_of_range("Axis out of range");

    Shape newShape;
    for (size_t i = 0; i < rank(); ++i) {
        if (i != axis) newShape.push_back(shape_[i]);
    }
    if (newShape.empty()) newShape.push_back(1);

    Tensor result = Tensor::fill(newShape, std::numeric_limits<DataType>::max());
    for (size_t i = 0; i < data_.size(); ++i) {
        auto idx = unflatIndex(i);
        std::vector<size_t> newIdx;
        for (size_t j = 0; j < idx.size(); ++j) {
            if (j != axis) newIdx.push_back(idx[j]);
        }
        if (newIdx.empty()) newIdx.push_back(0);
        result.at(newIdx) = std::min(result.at(newIdx), data_[i]);
    }
    return result;
}

Tensor Tensor::max(size_t axis) const {
    if (axis >= rank()) throw std::out_of_range("Axis out of range");

    Shape newShape;
    for (size_t i = 0; i < rank(); ++i) {
        if (i != axis) newShape.push_back(shape_[i]);
    }
    if (newShape.empty()) newShape.push_back(1);

    Tensor result = Tensor::fill(newShape, std::numeric_limits<DataType>::lowest());
    for (size_t i = 0; i < data_.size(); ++i) {
        auto idx = unflatIndex(i);
        std::vector<size_t> newIdx;
        for (size_t j = 0; j < idx.size(); ++j) {
            if (j != axis) newIdx.push_back(idx[j]);
        }
        if (newIdx.empty()) newIdx.push_back(0);
        result.at(newIdx) = std::max(result.at(newIdx), data_[i]);
    }
    return result;
}

// === Matrixoperationen ===

Tensor Tensor::matmul(const Tensor& other) const {
    if (rank() != 2 || other.rank() != 2) {
        throw std::invalid_argument("matmul requires 2D tensors");
    }
    if (shape_[1] != other.shape_[0]) {
        throw std::invalid_argument("Incompatible shapes for matmul");
    }

    size_t m = shape_[0];
    size_t n = shape_[1];
    size_t p = other.shape_[1];

    Tensor result({m, p});
    for (size_t i = 0; i < m; ++i) {
        for (size_t j = 0; j < p; ++j) {
            DataType sum = 0;
            for (size_t k = 0; k < n; ++k) {
                sum += at(i, k) * other.at(k, j);
            }
            result.at(i, j) = sum;
        }
    }
    return result;
}

Tensor Tensor::dot(const Tensor& other) const {
    if (rank() != 1 || other.rank() != 1) {
        throw std::invalid_argument("dot requires 1D tensors");
    }
    if (shape_[0] != other.shape_[0]) {
        throw std::invalid_argument("Vectors must have same length");
    }

    DataType result = 0;
    for (size_t i = 0; i < shape_[0]; ++i) {
        result += data_[i] * other[i];
    }
    return Tensor(result);
}

Tensor::DataType Tensor::norm() const {
    DataType sumSq = 0;
    for (DataType x : data_) {
        sumSq += x * x;
    }
    return std::sqrt(sumSq);
}

Tensor Tensor::normalize() const {
    DataType n = norm();
    if (n == 0) return *this;
    return *this / n;
}

// === Vergleiche ===

bool Tensor::operator==(const Tensor& other) const {
    return shape_ == other.shape_ && data_ == other.data_;
}

bool Tensor::operator!=(const Tensor& other) const {
    return !(*this == other);
}

bool Tensor::allClose(const Tensor& other, DataType rtol, DataType atol) const {
    if (shape_ != other.shape_) return false;
    for (size_t i = 0; i < data_.size(); ++i) {
        DataType diff = std::abs(data_[i] - other[i]);
        if (diff > atol + rtol * std::abs(other[i])) {
            return false;
        }
    }
    return true;
}

// === String-Darstellung ===

std::string Tensor::toString() const {
    if (empty()) return "Tensor([])";
    if (rank() == 0) return "Tensor(" + std::to_string(data_[0]) + ")";

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(4);

    if (rank() == 1) {
        oss << "[";
        for (size_t i = 0; i < shape_[0]; ++i) {
            if (i > 0) oss << ", ";
            oss << data_[i];
        }
        oss << "]";
    } else if (rank() == 2) {
        oss << "[";
        for (size_t i = 0; i < shape_[0]; ++i) {
            if (i > 0) oss << " ";
            oss << "[";
            for (size_t j = 0; j < shape_[1]; ++j) {
                if (j > 0) oss << ", ";
                oss << at(i, j);
            }
            oss << "]";
            if (i < shape_[0] - 1) oss << ",\n";
        }
        oss << "]";
    } else {
        oss << "Tensor(shape=" << shapeString() << ", data=[...])";
    }

    return oss.str();
}

std::string Tensor::toDetailedString() const {
    std::ostringstream oss;
    oss << "Tensor {\n";
    oss << "  shape: " << shapeString() << "\n";
    oss << "  rank: " << rank() << "\n";
    oss << "  size: " << size() << " elements\n";
    if (!empty()) {
        oss << "  min: " << min() << "\n";
        oss << "  max: " << max() << "\n";
        oss << "  mean: " << mean() << "\n";
    }
    oss << "  data: " << toString() << "\n";
    oss << "}";
    return oss.str();
}

std::string Tensor::shapeString() const {
    std::ostringstream oss;
    oss << "(";
    for (size_t i = 0; i < shape_.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << shape_[i];
    }
    oss << ")";
    return oss.str();
}

// === Visualisierungs-Hilfsmethoden ===

std::vector<Tensor::DataType> Tensor::normalizedData() const {
    if (empty()) return {};
    DataType minVal = min();
    DataType maxVal = max();
    DataType range = maxVal - minVal;
    if (range == 0) range = 1;

    std::vector<DataType> result(data_.size());
    for (size_t i = 0; i < data_.size(); ++i) {
        result[i] = (data_[i] - minVal) / range;
    }
    return result;
}

std::vector<Tensor::Point3D> Tensor::get3DPositions(float spacing) const {
    std::vector<Point3D> positions;
    positions.reserve(data_.size());

    for (size_t i = 0; i < data_.size(); ++i) {
        auto idx = unflatIndex(i);
        Point3D p{0, 0, 0};
        if (idx.size() >= 1) p.x = idx[0] * spacing;
        if (idx.size() >= 2) p.y = idx[1] * spacing;
        if (idx.size() >= 3) p.z = idx[2] * spacing;
        positions.push_back(p);
    }

    return positions;
}

// === Private Hilfsmethoden ===

void Tensor::computeStrides() {
    strides_.resize(shape_.size());
    if (shape_.empty()) return;

    strides_.back() = 1;
    for (int i = static_cast<int>(shape_.size()) - 2; i >= 0; --i) {
        strides_[i] = strides_[i + 1] * shape_[i + 1];
    }
}

size_t Tensor::flatIndex(const std::vector<size_t>& indices) const {
    size_t idx = 0;
    for (size_t i = 0; i < indices.size(); ++i) {
        idx += indices[i] * strides_[i];
    }
    return idx;
}

std::vector<size_t> Tensor::unflatIndex(size_t flatIdx) const {
    std::vector<size_t> indices(shape_.size());
    for (size_t i = 0; i < shape_.size(); ++i) {
        indices[i] = flatIdx / strides_[i];
        flatIdx %= strides_[i];
    }
    return indices;
}

void Tensor::validateShape(const Shape& shape) const {
    for (size_t d : shape) {
        if (d == 0) {
            throw std::invalid_argument("Shape dimensions must be positive");
        }
    }
}

void Tensor::validateIndices(const std::vector<size_t>& indices) const {
    if (indices.size() != shape_.size()) {
        throw std::invalid_argument("Number of indices doesn't match rank");
    }
    for (size_t i = 0; i < indices.size(); ++i) {
        if (indices[i] >= shape_[i]) {
            throw std::out_of_range("Index out of bounds");
        }
    }
}

void Tensor::broadcastShapes(const Tensor& other, Shape& resultShape) const {
    // Simplified broadcasting - shapes must match for now
    if (shape_ != other.shape_) {
        throw std::invalid_argument("Shape mismatch (broadcasting not fully implemented)");
    }
    resultShape = shape_;
}

// === Freie Funktionen ===

Tensor matmul(const Tensor& a, const Tensor& b) {
    return a.matmul(b);
}

Tensor concatenate(const std::vector<Tensor>& tensors, size_t axis) {
    if (tensors.empty()) {
        throw std::invalid_argument("Cannot concatenate empty list");
    }

    // Berechne neue Form
    Tensor::Shape newShape = tensors[0].shape();
    for (size_t i = 1; i < tensors.size(); ++i) {
        if (tensors[i].rank() != tensors[0].rank()) {
            throw std::invalid_argument("All tensors must have same rank");
        }
        for (size_t j = 0; j < newShape.size(); ++j) {
            if (j == axis) {
                newShape[j] += tensors[i].shape()[j];
            } else if (tensors[i].shape()[j] != newShape[j]) {
                throw std::invalid_argument("Shape mismatch on non-concat axis");
            }
        }
    }

    Tensor result(newShape);
    size_t offset = 0;
    for (const auto& t : tensors) {
        for (size_t i = 0; i < t.size(); ++i) {
            auto idx = result.shape(); // just for sizing
            // Simple copy for axis 0
            if (axis == 0) {
                result[offset + i] = t[i];
            }
        }
        offset += t.size();
    }

    return result;
}

Tensor stack(const std::vector<Tensor>& tensors, size_t axis) {
    if (tensors.empty()) {
        throw std::invalid_argument("Cannot stack empty list");
    }

    Tensor::Shape newShape = tensors[0].shape();
    newShape.insert(newShape.begin() + axis, tensors.size());

    Tensor result(newShape);
    for (size_t i = 0; i < tensors.size(); ++i) {
        for (size_t j = 0; j < tensors[i].size(); ++j) {
            // Vereinfachte Implementierung
            result[i * tensors[i].size() + j] = tensors[i][j];
        }
    }

    return result;
}

} // namespace tensor
