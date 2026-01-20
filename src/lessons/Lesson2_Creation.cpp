#include "lessons/Lesson.hpp"

namespace lessons {

Lesson2_Creation::Lesson2_Creation()
    : Lesson("Tensoren erstellen", "Lerne verschiedene Wege, Tensoren in C++ zu erstellen") {}

void Lesson2_Creation::init() {
    // Schritt 1: Konstruktoren
    addStep({
        "Tensor-Konstruktoren",
        "Es gibt mehrere Wege, einen Tensor zu erstellen. Der einfachste ist der Konstruktor mit Shape.\n\n"
        "Ein Tensor mit Shape (4) wird automatisch mit Nullen initialisiert.\n\n"
        "Du kannst auch direkt Werte übergeben!",
        "// Leerer Tensor\n"
        "Tensor t1;\n"
        "\n"
        "// Mit Shape (nullinitialisiert)\n"
        "Tensor t2({4});      // Vektor\n"
        "Tensor t3({3, 3});   // Matrix\n"
        "Tensor t4({2,3,4});  // 3D\n"
        "\n"
        "// Mit Daten\n"
        "Tensor t5({3}, {1.0f, 2.0f, 3.0f});",
        []() { return tensor::Tensor({4}); },
        gui::VisualizationMode::Bars
    });

    // Schritt 2: Fabrikmethoden - zeros/ones
    addStep({
        "Zeros und Ones",
        "Die statischen Fabrikmethoden 'zeros' und 'ones' sind sehr praktisch.\n\n"
        "zeros() erstellt einen Tensor voller Nullen.\n"
        "ones() erstellt einen Tensor voller Einsen.\n\n"
        "Das ist nützlich für Initialisierungen!",
        "// Alles Nullen\n"
        "auto zeros = Tensor::zeros({3, 3});\n"
        "\n"
        "// Alles Einsen\n"
        "auto ones = Tensor::ones({3, 3});\n"
        "\n"
        "// Beliebiger Wert\n"
        "auto filled = Tensor::fill({3,3}, 5.0f);",
        []() { return tensor::Tensor::ones({4, 4}); },
        gui::VisualizationMode::Heatmap
    });

    // Schritt 3: Random
    addStep({
        "Zufällige Tensoren",
        "Für Machine Learning braucht man oft zufällig initialisierte Tensoren.\n\n"
        "random() erstellt Tensoren mit gleichverteilten Zufallswerten.\n\n"
        "Du kannst den Bereich mit min und max angeben!",
        "// Zufaellig [0, 1]\n"
        "auto r1 = Tensor::random({4, 4});\n"
        "\n"
        "// Zufaellig [-1, 1]\n"
        "auto r2 = Tensor::random({4,4}, -1.0f, 1.0f);\n"
        "\n"
        "// Jeder Aufruf gibt andere Werte!",
        []() { return tensor::Tensor::random({5, 5}); },
        gui::VisualizationMode::Heatmap
    });

    // Schritt 4: Range
    addStep({
        "Range - Wertesequenz",
        "range() erstellt einen Vektor mit einer Zahlenfolge.\n\n"
        "Ähnlich wie range() in Python!\n\n"
        "Parameter: start, end, step (optional).",
        "// 1, 2, 3, 4, 5\n"
        "auto r1 = Tensor::range(1, 6);\n"
        "\n"
        "// 0, 2, 4, 6, 8\n"
        "auto r2 = Tensor::range(0, 10, 2);\n"
        "\n"
        "// 10, 9, 8, 7, 6\n"
        "auto r3 = Tensor::range(10, 5, -1);",
        []() { return tensor::Tensor::range(1, 11); },
        gui::VisualizationMode::Bars
    });

    // Schritt 5: Identity
    addStep({
        "Einheitsmatrix",
        "identity(n) erstellt eine n×n Einheitsmatrix.\n\n"
        "Einheitsmatrizen haben Einsen auf der Diagonale und Nullen sonst.\n\n"
        "Sie sind das neutrale Element der Matrixmultiplikation!",
        "// 3x3 Einheitsmatrix\n"
        "auto I = Tensor::identity(3);\n"
        "\n"
        "// I = [1, 0, 0]\n"
        "//     [0, 1, 0]\n"
        "//     [0, 0, 1]\n"
        "\n"
        "// A * I = A (fuer jede Matrix A)",
        []() { return tensor::Tensor::identity(5); },
        gui::VisualizationMode::Heatmap
    });

    // Schritt 6: Aus Vektoren/Matrizen
    addStep({
        "Aus C++ Containern",
        "Du kannst Tensoren direkt aus std::vector erstellen.\n\n"
        "fromVector() für 1D Daten.\n"
        "fromMatrix() für 2D Daten (verschachtelte Vektoren).\n\n"
        "Praktisch für existierende Daten!",
        "// Aus std::vector\n"
        "std::vector<float> v = {1,2,3,4,5};\n"
        "auto t1 = Tensor::fromVector(v);\n"
        "\n"
        "// Aus verschachteltem vector\n"
        "std::vector<std::vector<float>> m = {\n"
        "    {1, 2, 3},\n"
        "    {4, 5, 6}\n"
        "};\n"
        "auto t2 = Tensor::fromMatrix(m);",
        []() {
            return tensor::Tensor::fromMatrix({
                {1, 2, 3, 4},
                {5, 6, 7, 8},
                {9, 10, 11, 12}
            });
        },
        gui::VisualizationMode::Heatmap
    });

    // Schritt 7: Lambda-Initialisierung
    addStep({
        "Lambda-Initialisierung",
        "Für komplexere Muster kannst du eine Lambda-Funktion übergeben.\n\n"
        "Die Funktion erhält den linearen Index und gibt den Wert zurück.\n\n"
        "Sehr flexibel für mathematische Muster!",
        "// Mit Lambda-Funktion\n"
        "Tensor t({4, 4}, [](size_t i) {\n"
        "    return static_cast<float>(i * i);\n"
        "});\n"
        "\n"
        "// Sinus-Welle\n"
        "Tensor wave({100}, [](size_t i) {\n"
        "    return sin(i * 0.1f);\n"
        "});",
        []() {
            return tensor::Tensor({6, 6}, [](size_t i) {
                return static_cast<float>(i % 7) / 6.0f;
            });
        },
        gui::VisualizationMode::Heatmap
    });

    // Schritt 8: Zusammenfassung
    addStep({
        "Zusammenfassung",
        "Du kennst jetzt alle wichtigen Wege, Tensoren zu erstellen:\n\n"
        "• Konstruktoren für Shape und Daten\n"
        "• zeros(), ones(), fill() für Initialisierung\n"
        "• random() für Zufallswerte\n"
        "• range() für Sequenzen\n"
        "• identity() für Einheitsmatrizen\n"
        "• fromVector/Matrix() für C++ Container\n"
        "• Lambda für komplexe Muster",
        "// Schnellreferenz:\n"
        "Tensor({3,3});           // Nullen\n"
        "Tensor::zeros({3,3});    // Nullen\n"
        "Tensor::ones({3,3});     // Einsen\n"
        "Tensor::fill({3,3}, x);  // Wert x\n"
        "Tensor::random({3,3});   // Zufall\n"
        "Tensor::range(0, 10);    // Sequenz\n"
        "Tensor::identity(3);     // I-Matrix",
        []() { return tensor::Tensor::random({4, 4, 4}); },
        gui::VisualizationMode::Cubes
    });

    Lesson::init();
}

void Lesson2_Creation::update(float deltaTime) {
    Lesson::update(deltaTime);

    // Hier könnten interaktive Elemente sein
}

} // namespace lessons
