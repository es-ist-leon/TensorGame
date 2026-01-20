#include "lessons/Lesson.hpp"
#include "tensor/TensorDB.hpp"

namespace lessons {

Lesson4_Database::Lesson4_Database()
    : Lesson("Tensor-Datenbanken", "Verwalte und organisiere Tensoren effizient") {}

void Lesson4_Database::init() {
    // Schritt 1: Was ist eine Tensor-DB?
    addStep({
        "Was ist eine Tensor-Datenbank?",
        "Eine Tensor-Datenbank speichert und verwaltet Tensoren:\n\n"
        "• Benennung und Organisation\n"
        "• Metadaten (Beschreibung, Tags)\n"
        "• Schneller Zugriff und Abfragen\n"
        "• Persistenz (Speichern/Laden)\n\n"
        "Wichtig für ML-Workflows!",
        "// TensorDB Uebersicht\n"
        "TensorDB db;\n"
        "\n"
        "// Speichern\n"
        "db.store(\"name\", tensor, \"beschreibung\");\n"
        "\n"
        "// Abrufen\n"
        "auto t = db.get(\"name\");\n"
        "\n"
        "// Metadaten, Abfragen, Persistenz...",
        []() { return tensor::Tensor::random({3, 3, 3}); },
        gui::VisualizationMode::Cubes
    });

    // Schritt 2: CRUD Operationen
    addStep({
        "CRUD Operationen",
        "Grundlegende Datenbankoperationen:\n\n"
        "Create - store()\n"
        "Read - get()\n"
        "Update - update()\n"
        "Delete - remove()\n\n"
        "Einfach und intuitiv!",
        "// CRUD Operationen\n"
        "TensorDB db;\n"
        "\n"
        "// CREATE\n"
        "db.store(\"weights\", Tensor::random({10,10}));\n"
        "\n"
        "// READ\n"
        "auto weights = db.get(\"weights\");\n"
        "\n"
        "// UPDATE\n"
        "db.update(\"weights\", newWeights);\n"
        "\n"
        "// DELETE\n"
        "db.remove(\"weights\");",
        []() { return tensor::Tensor::random({4, 4}); },
        gui::VisualizationMode::Heatmap
    });

    // Schritt 3: Metadaten
    addStep({
        "Metadaten verwalten",
        "Jeder Tensor kann Metadaten haben:\n\n"
        "• Name und Beschreibung\n"
        "• Shape und Größe\n"
        "• Erstellungs-/Änderungsdatum\n"
        "• Benutzerdefinierte Tags\n\n"
        "Hilft bei Organisation großer Datensätze!",
        "// Metadaten\n"
        "db.store(\"model_v1\", weights,\n"
        "         \"Trainierte Gewichte v1\");\n"
        "\n"
        "// Tags setzen\n"
        "db.setTag(\"model_v1\", \"version\", \"1.0\");\n"
        "db.setTag(\"model_v1\", \"type\", \"weights\");\n"
        "\n"
        "// Metadaten abrufen\n"
        "auto meta = db.getMetadata(\"model_v1\");\n"
        "// meta->shape, meta->created, etc.",
        []() { return tensor::Tensor::random({5, 5}); },
        gui::VisualizationMode::Heatmap
    });

    // Schritt 4: Abfragen
    addStep({
        "Abfragen & Filtern",
        "Finde Tensoren nach Kriterien:\n\n"
        "• Nach Shape filtern\n"
        "• Nach Rang filtern\n"
        "• Nach Tags suchen\n"
        "• Alle Namen auflisten\n\n"
        "Schneller Zugriff auf relevante Daten!",
        "// Abfragen\n"
        "\n"
        "// Alle Namen\n"
        "auto names = db.listNames();\n"
        "\n"
        "// Nach Shape\n"
        "auto matrices = db.findByShape({3, 3});\n"
        "\n"
        "// Nach Rang\n"
        "auto vectors = db.findByRank(1);\n"
        "\n"
        "// Nach Tag\n"
        "auto v1 = db.findByTag(\"version\", \"1.0\");",
        []() { return tensor::Tensor::identity(4); },
        gui::VisualizationMode::Heatmap
    });

    // Schritt 5: Berechnungen
    addStep({
        "Berechnungen in der DB",
        "Die DB kann Operationen direkt ausführen:\n\n"
        "compute() führt Operationen zwischen\n"
        "gespeicherten Tensoren aus und speichert\n"
        "das Ergebnis direkt in der DB.\n\n"
        "Kein manuelles Laden/Speichern nötig!",
        "// Berechnungen\n"
        "db.store(\"a\", Tensor::ones({3,3}));\n"
        "db.store(\"b\", Tensor::ones({3,3}) * 2);\n"
        "\n"
        "// Berechne und speichere Ergebnis\n"
        "db.compute(\"sum\", \"a\", \"b\", \"add\");\n"
        "db.compute(\"prod\", \"a\", \"b\", \"matmul\");\n"
        "\n"
        "// Operationen: add, sub, mul, div, matmul",
        []() {
            auto a = tensor::Tensor::ones({3, 3});
            auto b = tensor::Tensor::ones({3, 3}) * 2.0f;
            return a + b;
        },
        gui::VisualizationMode::Heatmap
    });

    // Schritt 6: Persistenz
    addStep({
        "Speichern & Laden",
        "Tensoren persistent speichern:\n\n"
        "saveToFile() - Gesamte DB in Datei\n"
        "loadFromFile() - DB aus Datei laden\n\n"
        "Ermöglicht Wiederverwendung von\n"
        "trainierten Modellen, Checkpoints, etc.",
        "// Persistenz\n"
        "TensorDB db;\n"
        "\n"
        "// Tensoren hinzufuegen...\n"
        "db.store(\"model\", weights);\n"
        "db.store(\"bias\", bias);\n"
        "\n"
        "// Speichern\n"
        "db.saveToFile(\"model.tdb\");\n"
        "\n"
        "// Spaeter laden\n"
        "TensorDB db2;\n"
        "db2.loadFromFile(\"model.tdb\");",
        []() { return tensor::Tensor::random({4, 4}); },
        gui::VisualizationMode::Memory
    });

    // Schritt 7: Statistiken
    addStep({
        "Datenbank-Statistiken",
        "Übersicht über den DB-Inhalt:\n\n"
        "• Anzahl Tensoren\n"
        "• Gesamtzahl Elemente\n"
        "• Speicherverbrauch\n"
        "• Verteilung nach Rang\n\n"
        "Hilft bei Ressourcen-Management!",
        "// Statistiken\n"
        "auto stats = db.getStats();\n"
        "\n"
        "stats.tensorCount;      // Anzahl\n"
        "stats.totalElements;    // Elemente\n"
        "stats.totalMemoryBytes; // Bytes\n"
        "\n"
        "// Rang-Verteilung\n"
        "stats.rankDistribution[0]; // Skalare\n"
        "stats.rankDistribution[1]; // Vektoren\n"
        "stats.rankDistribution[2]; // Matrizen",
        []() { return tensor::Tensor::random({3, 3, 3}); },
        gui::VisualizationMode::Network
    });

    // Schritt 8: Anwendungsbeispiel ML
    addStep({
        "Praxisbeispiel: ML Workflow",
        "Typischer Machine Learning Workflow:\n\n"
        "1. Trainingsdaten laden/speichern\n"
        "2. Modell-Gewichte verwalten\n"
        "3. Checkpoints erstellen\n"
        "4. Verschiedene Versionen taggen\n"
        "5. Ergebnisse organisieren",
        "// ML Workflow\n"
        "TensorDB db;\n"
        "\n"
        "// Daten speichern\n"
        "db.store(\"X_train\", trainData);\n"
        "db.store(\"y_train\", labels);\n"
        "\n"
        "// Training...\n"
        "\n"
        "// Checkpoint\n"
        "db.store(\"weights_epoch10\", weights);\n"
        "db.setTag(\"weights_epoch10\", \"epoch\", \"10\");\n"
        "\n"
        "// Bestes Modell finden\n"
        "auto best = db.findByTag(\"best\", \"true\");",
        []() { return tensor::Tensor::random({8, 8}); },
        gui::VisualizationMode::Heatmap
    });

    // Schritt 9: Zusammenfassung
    addStep({
        "Zusammenfassung",
        "Tensor-Datenbank Konzepte:\n\n"
        "• CRUD: store, get, update, remove\n"
        "• Metadaten: Beschreibung, Tags\n"
        "• Abfragen: Shape, Rang, Tags\n"
        "• Berechnungen: compute()\n"
        "• Persistenz: save/loadFromFile\n"
        "• Statistiken: getStats()\n\n"
        "Probiere die Datenbank-Ansicht aus!",
        "// TensorDB Schnellreferenz\n"
        "TensorDB db;\n"
        "\n"
        "db.store(name, tensor, desc);\n"
        "db.get(name);\n"
        "db.update(name, newTensor);\n"
        "db.remove(name);\n"
        "\n"
        "db.setTag(name, key, value);\n"
        "db.findByShape/Rank/Tag(...);\n"
        "db.compute(result, a, b, op);\n"
        "\n"
        "db.saveToFile(path);\n"
        "db.loadFromFile(path);",
        []() { return tensor::Tensor::random({4, 4, 4}); },
        gui::VisualizationMode::Cubes
    });

    Lesson::init();
}

} // namespace lessons
