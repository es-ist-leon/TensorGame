#include "lessons/Lesson.hpp"

namespace lessons {

Lesson3_Operations::Lesson3_Operations()
    : Lesson("Tensor-Operationen", "Lerne wie du mit Tensoren rechnest") {}

void Lesson3_Operations::init() {
    // Initial Tensors for operations
    tensorA_ = tensor::Tensor::fromMatrix({{1, 2}, {3, 4}});
    tensorB_ = tensor::Tensor::fromMatrix({{5, 6}, {7, 8}});

    // Schritt 1: Elementweise Addition
    addStep({
        "Elementweise Addition",
        "Die einfachste Operation: Element für Element addieren.\n\n"
        "Beide Tensoren müssen die gleiche Shape haben!\n\n"
        "Das Ergebnis hat ebenfalls die gleiche Shape.",
        "// Elementweise Addition\n"
        "Tensor a({2,2}, {1,2,3,4});\n"
        "Tensor b({2,2}, {5,6,7,8});\n"
        "\n"
        "Tensor c = a + b;\n"
        "// c = [1+5, 2+6] = [6, 8]\n"
        "//     [3+7, 4+8]   [10, 12]",
        []() {
            auto a = tensor::Tensor::fromMatrix({{1, 2}, {3, 4}});
            auto b = tensor::Tensor::fromMatrix({{5, 6}, {7, 8}});
            return a + b;
        },
        gui::VisualizationMode::Heatmap
    });

    // Schritt 2: Subtraktion
    addStep({
        "Elementweise Subtraktion",
        "Subtraktion funktioniert genauso wie Addition.\n\n"
        "a - b subtrahiert jedes Element von b von a.\n\n"
        "Auch hier müssen die Shapes übereinstimmen.",
        "// Elementweise Subtraktion\n"
        "Tensor a({2,2}, {5,6,7,8});\n"
        "Tensor b({2,2}, {1,2,3,4});\n"
        "\n"
        "Tensor c = a - b;\n"
        "// c = [5-1, 6-2] = [4, 4]\n"
        "//     [7-3, 8-4]   [4, 4]",
        []() {
            auto a = tensor::Tensor::fromMatrix({{5, 6}, {7, 8}});
            auto b = tensor::Tensor::fromMatrix({{1, 2}, {3, 4}});
            return a - b;
        },
        gui::VisualizationMode::Heatmap
    });

    // Schritt 3: Elementweise Multiplikation
    addStep({
        "Elementweise Multiplikation",
        "ACHTUNG: Der * Operator macht elementweise Multiplikation!\n\n"
        "Das ist NICHT Matrixmultiplikation!\n\n"
        "Jedes Element wird mit dem entsprechenden Element multipliziert.",
        "// Elementweise Multiplikation\n"
        "Tensor a({2,2}, {1,2,3,4});\n"
        "Tensor b({2,2}, {2,2,2,2});\n"
        "\n"
        "Tensor c = a * b;\n"
        "// c = [1*2, 2*2] = [2, 4]\n"
        "//     [3*2, 4*2]   [6, 8]\n"
        "\n"
        "// Auch 'Hadamard-Produkt' genannt",
        []() {
            auto a = tensor::Tensor::fromMatrix({{1, 2}, {3, 4}});
            auto b = tensor::Tensor::fromMatrix({{2, 2}, {2, 2}});
            return a * b;
        },
        gui::VisualizationMode::Heatmap
    });

    // Schritt 4: Skalar-Operationen
    addStep({
        "Skalar-Operationen",
        "Du kannst einen Skalar mit einem Tensor verrechnen.\n\n"
        "Der Skalar wird auf jedes Element angewendet.\n\n"
        "Funktioniert für +, -, *, /.",
        "// Skalar-Operationen\n"
        "Tensor a({3}, {1, 2, 3});\n"
        "\n"
        "auto b = a + 10;  // [11, 12, 13]\n"
        "auto c = a * 2;   // [2, 4, 6]\n"
        "auto d = a / 2;   // [0.5, 1, 1.5]\n"
        "\n"
        "// Sehr nuetzlich fuer Normalisierung!",
        []() {
            auto a = tensor::Tensor::range(1, 6);
            return a * 2.0f;
        },
        gui::VisualizationMode::Bars
    });

    // Schritt 5: Matrixmultiplikation
    addStep({
        "Matrixmultiplikation",
        "Die ECHTE Matrixmultiplikation verwendet matmul().\n\n"
        "Für A(m×n) @ B(n×p) muss die innere Dimension übereinstimmen!\n"
        "Ergebnis hat Shape (m×p).\n\n"
        "Fundamentale Operation in Neural Networks!",
        "// Matrixmultiplikation\n"
        "Tensor A({2, 3});  // 2x3\n"
        "Tensor B({3, 2});  // 3x2\n"
        "\n"
        "Tensor C = A.matmul(B);  // 2x2!\n"
        "\n"
        "// C[i,j] = sum(A[i,:] * B[:,j])\n"
        "// Zeile von A dot Spalte von B",
        []() {
            auto A = tensor::Tensor::fromMatrix({{1, 2, 3}, {4, 5, 6}});
            auto B = tensor::Tensor::fromMatrix({{7, 8}, {9, 10}, {11, 12}});
            return A.matmul(B);
        },
        gui::VisualizationMode::Heatmap
    });

    // Schritt 6: Transpose
    addStep({
        "Transponieren",
        "transpose() spiegelt eine Matrix an der Diagonale.\n\n"
        "Zeilen werden zu Spalten und umgekehrt.\n"
        "Shape (m×n) wird zu (n×m).\n\n"
        "Wichtig für viele lineare Algebra Operationen!",
        "// Transponieren\n"
        "Tensor A({2, 3}, {1,2,3,4,5,6});\n"
        "// A = [1, 2, 3]\n"
        "//     [4, 5, 6]\n"
        "\n"
        "Tensor B = A.transpose();\n"
        "// B = [1, 4]\n"
        "//     [2, 5]\n"
        "//     [3, 6]",
        []() {
            auto A = tensor::Tensor::fromMatrix({{1, 2, 3, 4}, {5, 6, 7, 8}});
            return A.transpose();
        },
        gui::VisualizationMode::Heatmap
    });

    // Schritt 7: Reshape
    addStep({
        "Reshape",
        "reshape() ändert die Form ohne die Daten zu ändern.\n\n"
        "Die Gesamtzahl der Elemente muss gleich bleiben!\n"
        "z.B. (2,6) → (3,4) → (12) → (2,2,3)\n\n"
        "Ändert nur die Interpretation der Daten.",
        "// Reshape\n"
        "Tensor A({2, 6});  // 12 Elemente\n"
        "\n"
        "auto B = A.reshape({3, 4});   // OK\n"
        "auto C = A.reshape({12});     // OK\n"
        "auto D = A.reshape({2,2,3}); // OK\n"
        "\n"
        "// A.reshape({5, 3}); // FEHLER! 15 != 12",
        []() {
            auto A = tensor::Tensor::range(1, 13);
            return A.reshape({3, 4});
        },
        gui::VisualizationMode::Heatmap
    });

    // Schritt 8: Reduktionen
    addStep({
        "Reduktionen",
        "Reduktionen fassen Werte zusammen:\n\n"
        "sum() - Summe aller Elemente\n"
        "mean() - Durchschnitt\n"
        "min()/max() - Extremwerte\n\n"
        "Kann auch achsenweise angewendet werden!",
        "// Reduktionen\n"
        "Tensor A({3,3}, {1,2,3,4,5,6,7,8,9});\n"
        "\n"
        "float s = A.sum();   // 45\n"
        "float m = A.mean();  // 5\n"
        "float lo = A.min();  // 1\n"
        "float hi = A.max();  // 9\n"
        "\n"
        "// Achsenweise:\n"
        "Tensor rowSums = A.sum(0);  // pro Spalte\n"
        "Tensor colSums = A.sum(1);  // pro Zeile",
        []() {
            return tensor::Tensor::fromMatrix({
                {1, 2, 3},
                {4, 5, 6},
                {7, 8, 9}
            });
        },
        gui::VisualizationMode::Heatmap
    });

    // Schritt 9: Mathematische Funktionen
    addStep({
        "Mathematische Funktionen",
        "Elementweise mathematische Funktionen:\n\n"
        "sqrt(), exp(), log(), abs()\n"
        "sin(), cos(), pow()\n\n"
        "Werden auf jedes Element angewendet.",
        "// Mathematische Funktionen\n"
        "Tensor A({4}, {1, 4, 9, 16});\n"
        "\n"
        "auto sqrtA = A.sqrt();    // [1,2,3,4]\n"
        "auto expA = A.exp();      // e^x\n"
        "auto logA = A.log();      // ln(x)\n"
        "auto absA = A.abs();      // |x|\n"
        "auto sinA = A.sin();      // sin(x)\n"
        "auto powA = A.pow(2);     // x^2",
        []() {
            auto A = tensor::Tensor::range(1, 17);
            return A.sqrt();
        },
        gui::VisualizationMode::Bars
    });

    // Schritt 10: Zusammenfassung
    addStep({
        "Zusammenfassung",
        "Tensor-Operationen:\n\n"
        "• Elementweise: +, -, *, / (gleiche Shape!)\n"
        "• Skalar: tensor ⊕ skalar\n"
        "• matmul(): Echte Matrixmultiplikation\n"
        "• transpose(): Zeilen ↔ Spalten\n"
        "• reshape(): Form ändern\n"
        "• Reduktionen: sum, mean, min, max\n"
        "• Math: sqrt, exp, log, sin, cos, pow",
        "// Operationen Uebersicht:\n"
        "c = a + b;        // Elementweise\n"
        "c = a * 2;        // Skalar\n"
        "c = a.matmul(b);  // Matrix-Mult.\n"
        "c = a.transpose();\n"
        "c = a.reshape({2,3});\n"
        "x = a.sum();      // Reduktion\n"
        "c = a.sqrt();     // Math-Funktion",
        []() { return tensor::Tensor::random({4, 4, 4}); },
        gui::VisualizationMode::Cubes
    });

    Lesson::init();
}

void Lesson3_Operations::update(float deltaTime) {
    Lesson::update(deltaTime);
}

void Lesson3_Operations::render() {
    Lesson::render();
}

} // namespace lessons
