#include "lessons/Lesson.hpp"

namespace lessons {

/**
 * @brief Lektion 5: Broadcasting und erweiterte Shapes
 */
class Lesson5_Broadcasting : public Lesson {
public:
    Lesson5_Broadcasting()
        : Lesson("Broadcasting & Shapes", "Verstehe wie Tensoren automatisch erweitert werden") {}

    void init() override {
        // Schritt 1: Was ist Broadcasting?
        addStep({
            "Was ist Broadcasting?",
            "Broadcasting erlaubt Operationen zwischen Tensoren unterschiedlicher Shapes.\n\n"
            "Statt einen Fehler zu werfen, werden kleinere Tensoren automatisch 'erweitert', "
            "um zur größeren Shape zu passen.\n\n"
            "Dies ist fundamental für effiziente Berechnungen!",
            "// Ohne Broadcasting: Fehler!\n"
            "Tensor a({3, 3});  // 3x3 Matrix\n"
            "Tensor b({3});     // Vektor\n"
            "\n"
            "// Mit Broadcasting:\n"
            "// b wird zu [[b], [b], [b]] erweitert\n"
            "Tensor c = a + b;  // Funktioniert!",
            []() { return tensor::Tensor::random({3, 3}); },
            gui::VisualizationMode::Heatmap
        });

        // Schritt 2: Broadcasting-Regeln
        addStep({
            "Broadcasting-Regeln",
            "Broadcasting folgt klaren Regeln:\n\n"
            "1. Shapes werden von rechts verglichen\n"
            "2. Dimensionen sind kompatibel wenn:\n"
            "   - Sie gleich sind, ODER\n"
            "   - Eine davon 1 ist\n"
            "3. Fehlende Dimensionen werden links mit 1 aufgefüllt",
            "// Beispiel: (3, 4) + (4)\n"
            "// (4) wird zu (1, 4)\n"
            "// Dann: (3, 4) + (1, 4)\n"
            "// Beide kompatibel -> (3, 4)\n"
            "\n"
            "// Beispiel: (2, 3, 4) + (3, 1)\n"
            "// (3, 1) wird zu (1, 3, 1)\n"
            "// Ergebnis: (2, 3, 4)",
            []() {
                auto a = tensor::Tensor::ones({3, 4});
                auto b = tensor::Tensor::range(1, 5);
                return a;  // Visualisiere die Matrix
            },
            gui::VisualizationMode::Heatmap
        });

        // Schritt 3: Skalar Broadcasting
        addStep({
            "Skalar Broadcasting",
            "Der einfachste Fall: Ein Skalar wird auf jeden Wert angewendet.\n\n"
            "Der Skalar () wird zu jeder beliebigen Shape erweitert.\n\n"
            "Das haben wir schon bei Tensor + Skalar gesehen!",
            "// Skalar Broadcasting\n"
            "Tensor a({2, 3}, {1,2,3,4,5,6});\n"
            "\n"
            "// Skalar 10 wird zu:\n"
            "// [[10,10,10], [10,10,10]]\n"
            "Tensor b = a + 10;\n"
            "// b = [[11,12,13], [14,15,16]]",
            []() {
                auto a = tensor::Tensor::fromMatrix({{1, 2, 3}, {4, 5, 6}});
                return a + 10.0f;
            },
            gui::VisualizationMode::Heatmap
        });

        // Schritt 4: Vektor auf Matrix
        addStep({
            "Vektor auf Matrix",
            "Ein Vektor kann auf jede Zeile oder Spalte einer Matrix angewendet werden.\n\n"
            "Shape (n,) + Shape (m, n) = Shape (m, n)\n"
            "Der Vektor wird auf jede Zeile angewendet.",
            "// Vektor auf Matrix\n"
            "Tensor matrix({3, 4});  // 3x4\n"
            "Tensor vec({4});        // 4 Elemente\n"
            "\n"
            "// vec wird zu:\n"
            "// [[vec],\n"
            "//  [vec],\n"
            "//  [vec]]\n"
            "Tensor result = matrix + vec;  // 3x4",
            []() {
                auto matrix = tensor::Tensor::ones({3, 4});
                return matrix;
            },
            gui::VisualizationMode::Heatmap
        });

        // Schritt 5: Outer Product mit Broadcasting
        addStep({
            "Outer Product",
            "Broadcasting ermöglicht elegante Outer Products:\n\n"
            "Zwei Vektoren können zu einer Matrix kombiniert werden, "
            "indem man ihre Shapes anpasst.\n\n"
            "Shape (n, 1) * (1, m) = (n, m)",
            "// Outer Product\n"
            "Tensor a({3, 1}, {1, 2, 3});    // Spalte\n"
            "Tensor b({1, 4}, {1,2,3,4});    // Zeile\n"
            "\n"
            "// Ergebnis: 3x4 Matrix\n"
            "// [[1*1, 1*2, 1*3, 1*4],\n"
            "//  [2*1, 2*2, 2*3, 2*4],\n"
            "//  [3*1, 3*2, 3*3, 3*4]]",
            []() {
                // Simuliere Outer Product
                auto result = tensor::Tensor({3, 4});
                for (size_t i = 0; i < 3; ++i) {
                    for (size_t j = 0; j < 4; ++j) {
                        result.at(i, j) = (i + 1) * (j + 1);
                    }
                }
                return result;
            },
            gui::VisualizationMode::Heatmap
        });

        // Schritt 6: Squeeze und Unsqueeze
        addStep({
            "Squeeze & Unsqueeze",
            "Diese Operationen ändern die Shape ohne die Daten:\n\n"
            "squeeze() - Entfernt Dimensionen der Größe 1\n"
            "unsqueeze(axis) - Fügt Dimension der Größe 1 ein\n\n"
            "Nützlich um Shapes für Broadcasting vorzubereiten!",
            "// Squeeze: Entfernt 1er-Dimensionen\n"
            "Tensor a({1, 3, 1, 4});\n"
            "Tensor b = a.squeeze();  // (3, 4)\n"
            "\n"
            "// Unsqueeze: Fügt Dimension hinzu\n"
            "Tensor c({3, 4});\n"
            "Tensor d = c.unsqueeze(0);  // (1, 3, 4)\n"
            "Tensor e = c.unsqueeze(1);  // (3, 1, 4)",
            []() { return tensor::Tensor::random({3, 4}); },
            gui::VisualizationMode::Heatmap
        });

        // Schritt 7: Reshape für Broadcasting
        addStep({
            "Reshape für Broadcasting",
            "Manchmal muss man Tensoren umformen, damit Broadcasting funktioniert.\n\n"
            "reshape() ändert die Interpretation der Daten.\n"
            "Die Gesamtzahl der Elemente muss gleich bleiben!",
            "// Problem: (12,) + (3, 4) - inkompatibel!\n"
            "\n"
            "// Lösung: Reshape\n"
            "Tensor a = Tensor::range(1, 13);  // (12,)\n"
            "Tensor b = a.reshape({3, 4});     // (3, 4)\n"
            "Tensor c = a.reshape({4, 3});     // (4, 3)\n"
            "Tensor d = a.reshape({2, 2, 3}); // (2,2,3)",
            []() {
                auto a = tensor::Tensor::range(1, 13);
                return a.reshape({3, 4});
            },
            gui::VisualizationMode::Heatmap
        });

        // Schritt 8: Broadcasting Fehler
        addStep({
            "Broadcasting-Fehler",
            "Nicht alle Shapes sind kompatibel!\n\n"
            "Fehler tritt auf wenn:\n"
            "- Dimensionen weder gleich noch 1 sind\n"
            "- Keine sinnvolle Erweiterung möglich ist",
            "// FEHLER Beispiele:\n"
            "(3, 4) + (3,)   // 4 != 3, keine 1\n"
            "(2, 3) + (4, 3) // 2 != 4, keine 1\n"
            "(5,) + (3,)     // 5 != 3, keine 1\n"
            "\n"
            "// OK Beispiele:\n"
            "(3, 4) + (4,)   // 4 == 4\n"
            "(3, 4) + (1, 4) // 1 kann erweitert werden\n"
            "(3, 4) + (3, 1) // 1 kann erweitert werden",
            []() { return tensor::Tensor::random({3, 4}); },
            gui::VisualizationMode::Heatmap
        });

        // Schritt 9: Praktisches Beispiel - Normalisierung
        addStep({
            "Praxis: Normalisierung",
            "Broadcasting macht Normalisierung elegant:\n\n"
            "Subtrahiere den Mittelwert und teile durch Standardabweichung - "
            "aber pro Spalte/Zeile!\n\n"
            "Mit Broadcasting: Einzeiler!",
            "// Spaltenweise Normalisierung\n"
            "Tensor data({100, 4});  // 100 Samples, 4 Features\n"
            "\n"
            "// Mittelwert pro Spalte: (4,)\n"
            "Tensor mean = data.mean(0);\n"
            "\n"
            "// Broadcasting: (100, 4) - (4,) = (100, 4)\n"
            "Tensor centered = data - mean;\n"
            "\n"
            "// Analog für Standardabweichung",
            []() {
                auto data = tensor::Tensor::random({5, 4});
                return data;
            },
            gui::VisualizationMode::Heatmap
        });

        // Schritt 10: Zusammenfassung
        addStep({
            "Zusammenfassung",
            "Broadcasting-Konzepte:\n\n"
            "• Automatische Shape-Erweiterung\n"
            "• Regeln: Rechts vergleichen, 1 ist flexibel\n"
            "• Skalar → beliebige Shape\n"
            "• squeeze()/unsqueeze() für Shape-Anpassung\n"
            "• reshape() für Umstrukturierung\n"
            "• Ermöglicht eleganten, effizienten Code",
            "// Broadcasting Schnellreferenz:\n"
            "()     + (n,m)   -> (n, m)   // Skalar\n"
            "(m,)   + (n, m)  -> (n, m)   // Vektor\n"
            "(n, 1) + (1, m)  -> (n, m)   // Outer\n"
            "(1, m) + (n, m)  -> (n, m)   // Zeile\n"
            "(n, 1) + (n, m)  -> (n, m)   // Spalte\n"
            "\n"
            "// Achtung: (n,) != (n, 1) != (1, n)",
            []() { return tensor::Tensor::random({4, 4, 4}); },
            gui::VisualizationMode::Cubes
        });

        Lesson::init();
    }
};

// Factory-Funktion für LessonManager
std::unique_ptr<Lesson> createLesson5() {
    return std::make_unique<Lesson5_Broadcasting>();
}

} // namespace lessons
