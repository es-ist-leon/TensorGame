# TensorGame - Interaktives Tensor-Lernspiel

Ein interaktives 3D-Lernspiel zum Erlernen von Tensoren und Tensor-Operationen in C++. Entwickelt mit Raylib für moderne Grafik und intuitive Visualisierung.

![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)
![Raylib](https://img.shields.io/badge/Raylib-5.0-green.svg)
![License](https://img.shields.io/badge/License-MIT-yellow.svg)

## Features

### Lektionen
- **Lektion 1: Was sind Tensoren?** - Grundlagen, Rang, Shape, Skalare, Vektoren, Matrizen
- **Lektion 2: Tensoren erstellen** - Factory-Methoden (zeros, ones, random, range, identity)
- **Lektion 3: Tensor-Operationen** - Mathematische Operationen, Matrixmultiplikation, Reduktionen
- **Lektion 4: TensorDB** - Tensoren speichern, laden und verwalten
- **Lektion 5: Broadcasting** - Shape-Manipulation und Broadcasting-Regeln
- **Lektion 6: Neuronale Netze** - Grundlagen neuronaler Netze mit Tensoren

### 3D-Visualisierung
Sieben verschiedene Visualisierungsmodi für Tensoren:
- **Cubes** - 3D-Würfel für jeden Tensor-Wert
- **Spheres** - Kugeln mit größenbasierter Skalierung
- **Points** - Punktwolken-Darstellung
- **Heatmap** - Farbbasierte Wertdarstellung
- **Bars** - Balkendiagramm-Stil
- **Network** - Netzwerk-Visualisierung für neuronale Netze
- **Memory** - Lineare Speicher-Layout-Ansicht

### Quiz-System
- Multiple-Choice-Fragen
- Tensor-Shape-Rätsel
- Tensor-Wert-Berechnungen
- Code-Vervollständigung
- Wahr/Falsch-Fragen
- Animiertes Feedback und Punktesystem

### Achievement-System
- 40+ Achievements in 7 Kategorien
- 5 Stufen: Bronze, Silber, Gold, Platin, Diamant
- Fortschrittsverfolgung
- Persistente Speicherung

### Code-Editor
- Syntax-Highlighting für C++ und Tensor-Methoden
- Auto-Vervollständigung
- Live-Tensor-Vorschau
- Vorlagen für häufige Muster
- Undo/Redo-Funktionalität

### Sandbox-Modus
- Freies Experimentieren mit Tensoren
- Interaktive Tensor-Erstellung
- Echtzeit-Operationen
- Konsole mit Befehlen

## Installation

### Voraussetzungen
- CMake 3.16 oder höher
- C++17-kompatibler Compiler (GCC 8+, Clang 7+, MSVC 2019+)
- OpenGL-Unterstützung

### Build

```bash
# Repository klonen
git clone https://github.com/es-ist-leon/TensorGame.git
cd TensorGame

# Build-Verzeichnis erstellen
mkdir build && cd build

# CMake konfigurieren (Raylib wird automatisch heruntergeladen)
cmake ..

# Kompilieren
cmake --build . -j$(nproc)

# Ausführen
./TensorGame
```

### Windows (Visual Studio)

```bash
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
```

## Projektstruktur

```
TensorGame/
├── CMakeLists.txt
├── README.md
├── src/
│   ├── main.cpp                 # Einstiegspunkt
│   ├── tensor/
│   │   ├── Tensor.hpp/.cpp      # Tensor-Klasse
│   │   └── TensorDB.hpp/.cpp    # Tensor-Datenbank
│   ├── gui/
│   │   ├── Colors.hpp           # Farbpalette
│   │   ├── TensorVisualizer.hpp/.cpp  # 3D-Visualisierung
│   │   ├── UIComponents.hpp/.cpp      # UI-Elemente
│   │   └── Application.hpp/.cpp       # Hauptanwendung
│   ├── lessons/
│   │   ├── Lesson.hpp           # Basis-Klasse
│   │   ├── LessonManager.hpp/.cpp     # Lektionsverwaltung
│   │   └── Lesson1-6_*.cpp      # Einzelne Lektionen
│   ├── quiz/
│   │   └── Quiz.hpp/.cpp        # Quiz-System
│   ├── progress/
│   │   └── Achievements.hpp/.cpp # Achievement-System
│   ├── editor/
│   │   └── CodeEditor.hpp/.cpp  # Code-Editor
│   └── sandbox/
│       └── Sandbox.hpp/.cpp     # Sandbox-Modus
```

## Steuerung

### Allgemein
- **ESC** - Zurück / Menü
- **Maus** - UI-Interaktion

### 3D-Ansicht
- **Linke Maustaste + Ziehen** - Kamera rotieren
- **Mausrad** - Zoom
- **WASD** - Kamera bewegen
- **Q/E** - Kamera hoch/runter
- **1-7** - Visualisierungsmodus wechseln
- **Space** - Animation pausieren/fortsetzen
- **R** - Ansicht zurücksetzen

### Code-Editor
- **Strg+Z** - Rückgängig
- **Strg+Y** - Wiederholen
- **Strg+C** - Kopieren
- **Strg+V** - Einfügen
- **Tab** - Auto-Vervollständigung akzeptieren
- **F5** - Code ausführen

## Tensor-API Übersicht

### Tensor erstellen
```cpp
// Verschiedene Erstellungsmethoden
Tensor scalar = Tensor::zeros({});           // Skalar
Tensor vector = Tensor::ones({5});           // Vektor
Tensor matrix = Tensor::random({3, 3});      // Matrix
Tensor tensor3d = Tensor::range({2, 3, 4});  // 3D-Tensor
Tensor identity = Tensor::identity(4);        // Einheitsmatrix
```

### Operationen
```cpp
// Element-weise Operationen
Tensor c = a + b;
Tensor d = a * b;

// Matrix-Operationen
Tensor result = a.matmul(b);
Tensor transposed = a.transpose();
Tensor reshaped = a.reshape({2, 6});

// Reduktionen
float sum = a.sum();
float mean = a.mean();
float max = a.max();
```

### TensorDB
```cpp
TensorDB db;
db.store("weights", tensor, {"neural_net", "layer1"});
Tensor loaded = db.get("weights");
auto results = db.findByTag("neural_net");
db.saveToFile("model.tdb");
```

## Technologien

- **C++17** - Moderne C++-Features
- **Raylib 5.0** - Grafik und Fenster-Management
- **CMake** - Build-System mit FetchContent für Abhängigkeiten

## Lizenz

MIT License - siehe LICENSE-Datei für Details.

## Mitwirken

Beiträge sind willkommen! Bitte erstelle einen Pull Request oder öffne ein Issue für Vorschläge und Fehlerberichte.

---

Entwickelt mit Leidenschaft für Bildung und Tensor-Mathematik.
