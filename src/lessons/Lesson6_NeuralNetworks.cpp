#include "lessons/Lesson.hpp"

namespace lessons {

/**
 * @brief Lektion 6: Neural Network Grundlagen mit Tensoren
 */
class Lesson6_NeuralNetworks : public Lesson {
public:
    Lesson6_NeuralNetworks()
        : Lesson("Neural Networks", "Verstehe wie Tensoren in KI verwendet werden") {}

    void init() override {
        // Schritt 1: Einführung
        addStep({
            "Tensoren in Neural Networks",
            "Neural Networks basieren vollständig auf Tensor-Operationen!\n\n"
            "Jede Schicht ist im Kern eine Matrixmultiplikation plus Bias.\n\n"
            "Die Visualisierung zeigt einen einfachen Input-Tensor.",
            "// Neural Network Grundformel:\n"
            "// output = activation(input @ weights + bias)\n"
            "\n"
            "// Beispiel:\n"
            "Tensor input({1, 784});    // Bild 28x28 flach\n"
            "Tensor weights({784, 128}); // Gewichte\n"
            "Tensor bias({128});         // Bias\n"
            "\n"
            "// Vorwärtsdurchlauf\n"
            "auto z = input.matmul(weights) + bias;",
            []() {
                // Simuliere einen kleinen Input
                return tensor::Tensor::random({4, 4});
            },
            gui::VisualizationMode::Heatmap
        });

        // Schritt 2: Gewichte als Tensoren
        addStep({
            "Gewichte-Tensoren",
            "Gewichte verbinden Neuronen zwischen Schichten.\n\n"
            "Shape: (input_features, output_features)\n\n"
            "Jeder Wert repräsentiert die Stärke einer Verbindung.",
            "// Gewichte-Matrix\n"
            "// input: 4 Neuronen\n"
            "// output: 3 Neuronen\n"
            "\n"
            "Tensor W({4, 3});\n"
            "// W[i][j] = Verbindungsstärke\n"
            "//           von Input i zu Output j\n"
            "\n"
            "// Zufällige Initialisierung\n"
            "W = Tensor::random({4, 3}, -0.5f, 0.5f);",
            []() {
                return tensor::Tensor::random({4, 3}, -0.5f, 0.5f);
            },
            gui::VisualizationMode::Heatmap
        });

        // Schritt 3: Bias-Tensoren
        addStep({
            "Bias-Tensoren",
            "Bias wird zu jeder Ausgabe addiert (Broadcasting!).\n\n"
            "Shape: (output_features,)\n\n"
            "Ermöglicht dem Netzwerk, die Aktivierungsschwelle anzupassen.",
            "// Bias-Vektor\n"
            "Tensor bias({3});  // Ein Wert pro Output\n"
            "\n"
            "// Nach matmul:\n"
            "// z = input @ W      // Shape: (batch, 3)\n"
            "// z = z + bias       // Broadcasting!\n"
            "\n"
            "// bias wird auf jede Zeile addiert\n"
            "// (batch, 3) + (3,) -> (batch, 3)",
            []() {
                return tensor::Tensor::random({3});
            },
            gui::VisualizationMode::Bars
        });

        // Schritt 4: Batch Processing
        addStep({
            "Batch Processing",
            "In der Praxis verarbeiten wir viele Samples gleichzeitig.\n\n"
            "Batch-Dimension kommt zuerst!\n"
            "Shape: (batch_size, features)",
            "// Einzelnes Sample\n"
            "Tensor single({1, 784});  // 1x784\n"
            "\n"
            "// Batch von 32 Samples\n"
            "Tensor batch({32, 784});  // 32x784\n"
            "\n"
            "// Matmul funktioniert für beide!\n"
            "// single @ W -> (1, out)\n"
            "// batch @ W  -> (32, out)\n"
            "\n"
            "// GPU-Parallelisierung = Speed!",
            []() {
                return tensor::Tensor::random({8, 4});  // Mini-Batch
            },
            gui::VisualizationMode::Heatmap
        });

        // Schritt 5: Aktivierungsfunktionen
        addStep({
            "Aktivierungsfunktionen",
            "Aktivierungen sind elementweise Funktionen auf Tensoren.\n\n"
            "Sie bringen Nicht-Linearität ins Netzwerk.\n\n"
            "Ohne sie wäre das ganze Netzwerk nur eine lineare Transformation!",
            "// Wichtige Aktivierungen:\n"
            "\n"
            "// ReLU: max(0, x)\n"
            "auto relu = z.apply([](float x) {\n"
            "    return x > 0 ? x : 0;\n"
            "});\n"
            "\n"
            "// Sigmoid: 1 / (1 + e^-x)\n"
            "auto sigmoid = (-z).exp();\n"
            "sigmoid = Tensor::ones(z.shape()) / \n"
            "          (Tensor::ones(z.shape()) + sigmoid);",
            []() {
                // ReLU Visualisierung
                auto input = tensor::Tensor::random({5, 5}, -1.0f, 1.0f);
                return input.apply([](float x) { return x > 0 ? x : 0; });
            },
            gui::VisualizationMode::Heatmap
        });

        // Schritt 6: Vollständige Schicht
        addStep({
            "Vollständige Schicht",
            "Eine Dense/Linear Layer kombiniert alle Konzepte:\n\n"
            "1. Matrixmultiplikation (input @ weights)\n"
            "2. Bias-Addition (+ bias)\n"
            "3. Aktivierung (relu, sigmoid, etc.)",
            "// Dense Layer Implementierung\n"
            "class DenseLayer {\n"
            "    Tensor weights, bias;\n"
            "public:\n"
            "    DenseLayer(int in, int out) {\n"
            "        weights = Tensor::random({in, out});\n"
            "        bias = Tensor::zeros({out});\n"
            "    }\n"
            "    \n"
            "    Tensor forward(const Tensor& x) {\n"
            "        auto z = x.matmul(weights) + bias;\n"
            "        return relu(z);\n"
            "    }\n"
            "};",
            []() {
                auto input = tensor::Tensor::random({4, 4});
                auto weights = tensor::Tensor::random({4, 3});
                return input.matmul(weights);
            },
            gui::VisualizationMode::Heatmap
        });

        // Schritt 7: Mehrstufiges Netzwerk
        addStep({
            "Multi-Layer Network",
            "Ein tiefes Netzwerk stapelt mehrere Schichten:\n\n"
            "Input → Layer1 → Layer2 → ... → Output\n\n"
            "Jede Schicht transformiert die Daten weiter.",
            "// 3-Layer Netzwerk\n"
            "DenseLayer layer1(784, 256);  // 784->256\n"
            "DenseLayer layer2(256, 128);  // 256->128\n"
            "DenseLayer layer3(128, 10);   // 128->10\n"
            "\n"
            "// Vorwärtsdurchlauf\n"
            "Tensor forward(const Tensor& input) {\n"
            "    auto h1 = layer1.forward(input);\n"
            "    auto h2 = layer2.forward(h1);\n"
            "    auto out = layer3.forward(h2);\n"
            "    return out;\n"
            "}",
            []() {
                return tensor::Tensor::random({4, 4, 4});
            },
            gui::VisualizationMode::Network
        });

        // Schritt 8: Convolutional Layer Konzept
        addStep({
            "Convolution (Konzept)",
            "CNNs für Bilder nutzen lokale Filter statt volle Verbindungen.\n\n"
            "Ein Filter 'gleitet' über das Bild.\n"
            "Tensor-Shape: (batch, channels, height, width)",
            "// CNN Tensor Shapes\n"
            "Tensor image({1, 3, 28, 28});  // RGB Bild\n"
            "Tensor filter({16, 3, 3, 3});  // 16 Filter 3x3\n"
            "\n"
            "// Nach Convolution:\n"
            "// Output: (1, 16, 26, 26)\n"
            "// - 16 Feature Maps\n"
            "// - Größe reduziert durch Filter\n"
            "\n"
            "// Kernoperationen sind immer noch\n"
            "// Tensor-Multiplikationen!",
            []() {
                // Simuliere Feature Map
                return tensor::Tensor::random({4, 4});
            },
            gui::VisualizationMode::Heatmap
        });

        // Schritt 9: Loss und Gradients
        addStep({
            "Loss-Berechnung",
            "Der Loss misst, wie gut das Netzwerk ist.\n\n"
            "Auch der Loss ist eine Tensor-Operation!\n\n"
            "MSE: Mean Squared Error\n"
            "Cross-Entropy für Klassifikation",
            "// Mean Squared Error\n"
            "Tensor mse(const Tensor& pred, \n"
            "           const Tensor& target) {\n"
            "    auto diff = pred - target;\n"
            "    auto squared = diff * diff;\n"
            "    return Tensor(squared.mean());\n"
            "}\n"
            "\n"
            "// Verwendung:\n"
            "auto loss = mse(output, labels);\n"
            "// loss ist ein Skalar-Tensor",
            []() {
                auto pred = tensor::Tensor::random({4});
                auto target = tensor::Tensor::random({4});
                auto diff = pred - target;
                return diff * diff;  // Squared errors
            },
            gui::VisualizationMode::Bars
        });

        // Schritt 10: Zusammenfassung
        addStep({
            "Zusammenfassung",
            "Neural Networks und Tensoren:\n\n"
            "• Gewichte: 2D Tensoren (in × out)\n"
            "• Bias: 1D Tensoren (out,)\n"
            "• Matmul: Kernoperation jeder Schicht\n"
            "• Broadcasting: Bias-Addition\n"
            "• Aktivierungen: Elementweise Funktionen\n"
            "• Batches: Effiziente Parallelverarbeitung\n\n"
            "Jede KI besteht aus Tensor-Operationen!",
            "// Neural Network = Tensor Ops\n"
            "\n"
            "// Forward Pass:\n"
            "h = relu(x @ W1 + b1)   // Layer 1\n"
            "y = softmax(h @ W2 + b2) // Output\n"
            "\n"
            "// Backward Pass (Training):\n"
            "// Gradients = mehr Tensor Ops!\n"
            "dW = x.T @ dh            // Gradient\n"
            "W = W - lr * dW          // Update\n"
            "\n"
            "// Alles sind Tensor-Operationen!",
            []() {
                return tensor::Tensor::random({5, 5, 5});
            },
            gui::VisualizationMode::Network
        });

        Lesson::init();
    }
};

// Factory-Funktion
std::unique_ptr<Lesson> createLesson6() {
    return std::make_unique<Lesson6_NeuralNetworks>();
}

} // namespace lessons
