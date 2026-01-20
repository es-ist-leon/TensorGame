#include "tensor/TensorDB.hpp"
#include <sstream>
#include <iomanip>
#include <ctime>

namespace tensor {

// === TensorMetadata ===

std::string TensorMetadata::shapeString() const {
    std::ostringstream oss;
    oss << "(";
    for (size_t i = 0; i < shape.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << shape[i];
    }
    oss << ")";
    return oss.str();
}

std::string TensorMetadata::createdString() const {
    auto time = std::chrono::system_clock::to_time_t(created);
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string TensorMetadata::modifiedString() const {
    auto time = std::chrono::system_clock::to_time_t(modified);
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

// === TensorDB ===

void TensorDB::store(const std::string& name, const Tensor& tensor,
                     const std::string& description) {
    auto now = std::chrono::system_clock::now();

    TensorMetadata meta;
    meta.name = name;
    meta.description = description;
    meta.shape = tensor.shape();
    meta.size = tensor.size();
    meta.created = now;
    meta.modified = now;

    tensors_[name] = {tensor, meta};
}

std::optional<Tensor> TensorDB::get(const std::string& name) const {
    auto it = tensors_.find(name);
    if (it != tensors_.end()) {
        return it->second.tensor;
    }
    return std::nullopt;
}

Tensor& TensorDB::getRef(const std::string& name) {
    auto it = tensors_.find(name);
    if (it == tensors_.end()) {
        throw std::runtime_error("Tensor not found: " + name);
    }
    return it->second.tensor;
}

const Tensor& TensorDB::getRef(const std::string& name) const {
    auto it = tensors_.find(name);
    if (it == tensors_.end()) {
        throw std::runtime_error("Tensor not found: " + name);
    }
    return it->second.tensor;
}

bool TensorDB::update(const std::string& name, const Tensor& tensor) {
    auto it = tensors_.find(name);
    if (it == tensors_.end()) {
        return false;
    }

    it->second.tensor = tensor;
    it->second.metadata.shape = tensor.shape();
    it->second.metadata.size = tensor.size();
    it->second.metadata.modified = std::chrono::system_clock::now();
    return true;
}

bool TensorDB::remove(const std::string& name) {
    return tensors_.erase(name) > 0;
}

bool TensorDB::exists(const std::string& name) const {
    return tensors_.find(name) != tensors_.end();
}

std::vector<std::string> TensorDB::listNames() const {
    std::vector<std::string> names;
    names.reserve(tensors_.size());
    for (const auto& [name, _] : tensors_) {
        names.push_back(name);
    }
    return names;
}

void TensorDB::clear() {
    tensors_.clear();
}

std::optional<TensorMetadata> TensorDB::getMetadata(const std::string& name) const {
    auto it = tensors_.find(name);
    if (it != tensors_.end()) {
        return it->second.metadata;
    }
    return std::nullopt;
}

bool TensorDB::setTag(const std::string& name, const std::string& key, const std::string& value) {
    auto it = tensors_.find(name);
    if (it == tensors_.end()) {
        return false;
    }
    it->second.metadata.tags[key] = value;
    return true;
}

std::optional<std::string> TensorDB::getTag(const std::string& name, const std::string& key) const {
    auto it = tensors_.find(name);
    if (it == tensors_.end()) {
        return std::nullopt;
    }
    auto tagIt = it->second.metadata.tags.find(key);
    if (tagIt != it->second.metadata.tags.end()) {
        return tagIt->second;
    }
    return std::nullopt;
}

std::vector<std::string> TensorDB::findByShape(const Tensor::Shape& shape) const {
    std::vector<std::string> results;
    for (const auto& [name, entry] : tensors_) {
        if (entry.tensor.shape() == shape) {
            results.push_back(name);
        }
    }
    return results;
}

std::vector<std::string> TensorDB::findByRank(size_t rank) const {
    std::vector<std::string> results;
    for (const auto& [name, entry] : tensors_) {
        if (entry.tensor.rank() == rank) {
            results.push_back(name);
        }
    }
    return results;
}

std::vector<std::string> TensorDB::findByTag(const std::string& key, const std::string& value) const {
    std::vector<std::string> results;
    for (const auto& [name, entry] : tensors_) {
        auto tagIt = entry.metadata.tags.find(key);
        if (tagIt != entry.metadata.tags.end() && tagIt->second == value) {
            results.push_back(name);
        }
    }
    return results;
}

bool TensorDB::compute(const std::string& resultName,
                       const std::string& a, const std::string& b,
                       const std::string& operation) {
    auto tensorA = get(a);
    auto tensorB = get(b);

    if (!tensorA || !tensorB) {
        return false;
    }

    try {
        Tensor result;
        if (operation == "add" || operation == "+") {
            result = *tensorA + *tensorB;
        } else if (operation == "sub" || operation == "-") {
            result = *tensorA - *tensorB;
        } else if (operation == "mul" || operation == "*") {
            result = *tensorA * *tensorB;
        } else if (operation == "div" || operation == "/") {
            result = *tensorA / *tensorB;
        } else if (operation == "matmul" || operation == "@") {
            result = tensorA->matmul(*tensorB);
        } else {
            return false;
        }

        store(resultName, result, "Computed: " + a + " " + operation + " " + b);
        return true;
    } catch (...) {
        return false;
    }
}

bool TensorDB::apply(const std::string& name, std::function<void(Tensor&)> func) {
    auto it = tensors_.find(name);
    if (it == tensors_.end()) {
        return false;
    }

    func(it->second.tensor);
    it->second.metadata.modified = std::chrono::system_clock::now();
    return true;
}

bool TensorDB::saveToFile(const std::string& filename) const {
    std::ofstream file(filename, std::ios::binary);
    if (!file) return false;

    // Einfaches Format: Anzahl, dann für jeden Tensor: Name, Shape, Daten
    size_t count = tensors_.size();
    file.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (const auto& [name, entry] : tensors_) {
        // Name
        size_t nameLen = name.size();
        file.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
        file.write(name.data(), nameLen);

        // Description
        size_t descLen = entry.metadata.description.size();
        file.write(reinterpret_cast<const char*>(&descLen), sizeof(descLen));
        file.write(entry.metadata.description.data(), descLen);

        // Shape
        size_t rank = entry.tensor.rank();
        file.write(reinterpret_cast<const char*>(&rank), sizeof(rank));
        for (size_t dim : entry.tensor.shape()) {
            file.write(reinterpret_cast<const char*>(&dim), sizeof(dim));
        }

        // Daten
        size_t dataSize = entry.tensor.size();
        file.write(reinterpret_cast<const char*>(&dataSize), sizeof(dataSize));
        file.write(reinterpret_cast<const char*>(entry.tensor.data().data()),
                   dataSize * sizeof(Tensor::DataType));
    }

    return true;
}

bool TensorDB::loadFromFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) return false;

    clear();

    size_t count;
    file.read(reinterpret_cast<char*>(&count), sizeof(count));

    for (size_t i = 0; i < count; ++i) {
        // Name
        size_t nameLen;
        file.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        std::string name(nameLen, '\0');
        file.read(name.data(), nameLen);

        // Description
        size_t descLen;
        file.read(reinterpret_cast<char*>(&descLen), sizeof(descLen));
        std::string description(descLen, '\0');
        file.read(description.data(), descLen);

        // Shape
        size_t rank;
        file.read(reinterpret_cast<char*>(&rank), sizeof(rank));
        Tensor::Shape shape(rank);
        for (size_t j = 0; j < rank; ++j) {
            file.read(reinterpret_cast<char*>(&shape[j]), sizeof(size_t));
        }

        // Daten
        size_t dataSize;
        file.read(reinterpret_cast<char*>(&dataSize), sizeof(dataSize));
        std::vector<Tensor::DataType> data(dataSize);
        file.read(reinterpret_cast<char*>(data.data()),
                  dataSize * sizeof(Tensor::DataType));

        Tensor tensor(shape, data);
        store(name, tensor, description);
    }

    return true;
}

TensorDB::DBStats TensorDB::getStats() const {
    DBStats stats;
    stats.tensorCount = tensors_.size();
    stats.totalElements = 0;
    stats.totalMemoryBytes = 0;

    for (const auto& [_, entry] : tensors_) {
        stats.totalElements += entry.tensor.size();
        stats.totalMemoryBytes += entry.tensor.size() * sizeof(Tensor::DataType);
        stats.rankDistribution[entry.tensor.rank()]++;
    }

    return stats;
}

} // namespace tensor
