#pragma once

#include "tensor/Tensor.hpp"
#include <map>
#include <optional>
#include <fstream>
#include <chrono>

namespace tensor {

/**
 * @brief Metadaten für einen gespeicherten Tensor
 */
struct TensorMetadata {
    std::string name;
    std::string description;
    Tensor::Shape shape;
    size_t size;
    std::chrono::system_clock::time_point created;
    std::chrono::system_clock::time_point modified;
    std::map<std::string, std::string> tags;

    std::string shapeString() const;
    std::string createdString() const;
    std::string modifiedString() const;
};

/**
 * @brief Einfache In-Memory Tensor-Datenbank
 *
 * Speichert Tensoren mit Namen und Metadaten.
 * Unterstützt grundlegende CRUD-Operationen und Abfragen.
 */
class TensorDB {
public:
    TensorDB() = default;

    // === CRUD Operationen ===

    // Tensor speichern
    void store(const std::string& name, const Tensor& tensor,
               const std::string& description = "");

    // Tensor abrufen
    std::optional<Tensor> get(const std::string& name) const;
    Tensor& getRef(const std::string& name);
    const Tensor& getRef(const std::string& name) const;

    // Tensor aktualisieren
    bool update(const std::string& name, const Tensor& tensor);

    // Tensor löschen
    bool remove(const std::string& name);

    // Existenz prüfen
    bool exists(const std::string& name) const;

    // Alle Namen abrufen
    std::vector<std::string> listNames() const;

    // Anzahl der Tensoren
    size_t count() const { return tensors_.size(); }

    // Datenbank leeren
    void clear();

    // === Metadaten ===

    std::optional<TensorMetadata> getMetadata(const std::string& name) const;
    bool setTag(const std::string& name, const std::string& key, const std::string& value);
    std::optional<std::string> getTag(const std::string& name, const std::string& key) const;

    // === Abfragen ===

    // Nach Shape filtern
    std::vector<std::string> findByShape(const Tensor::Shape& shape) const;

    // Nach Rang filtern
    std::vector<std::string> findByRank(size_t rank) const;

    // Nach Tag filtern
    std::vector<std::string> findByTag(const std::string& key, const std::string& value) const;

    // === Operationen auf gespeicherten Tensoren ===

    // Führt Operation auf zwei Tensoren aus und speichert das Ergebnis
    bool compute(const std::string& resultName,
                 const std::string& a, const std::string& b,
                 const std::string& operation);

    // Anwenden einer Funktion
    bool apply(const std::string& name, std::function<void(Tensor&)> func);

    // === Persistenz (einfach) ===

    bool saveToFile(const std::string& filename) const;
    bool loadFromFile(const std::string& filename);

    // === Statistiken ===

    struct DBStats {
        size_t tensorCount;
        size_t totalElements;
        size_t totalMemoryBytes;
        std::map<size_t, size_t> rankDistribution;
    };

    DBStats getStats() const;

    // === Iterator-Unterstützung ===

    auto begin() { return tensors_.begin(); }
    auto end() { return tensors_.end(); }
    auto begin() const { return tensors_.begin(); }
    auto end() const { return tensors_.end(); }

private:
    struct TensorEntry {
        Tensor tensor;
        TensorMetadata metadata;
    };

    std::map<std::string, TensorEntry> tensors_;
};

} // namespace tensor
