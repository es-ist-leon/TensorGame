#pragma once

#include "raylib.h"
#include "gui/UIComponents.hpp"
#include "gui/TensorVisualizer.hpp"
#include "tensor/Tensor.hpp"
#include "tensor/TensorDB.hpp"
#include <string>
#include <vector>
#include <map>
#include <functional>

namespace editor {

/**
 * @brief Syntax-Token für Highlighting
 */
enum class TokenType {
    Unknown,
    Keyword,
    Type,
    Function,
    Number,
    String,
    Comment,
    Operator,
    Identifier,
    Bracket,
    TensorMethod
};

/**
 * @brief Ein Token im Code
 */
struct Token {
    std::string text;
    TokenType type;
    int startCol;
    int endCol;
};

/**
 * @brief Eine Zeile im Editor
 */
struct EditorLine {
    std::string text;
    std::vector<Token> tokens;
    bool hasError = false;
    std::string errorMessage;
};

/**
 * @brief Code-Vorschlag für Autovervollständigung
 */
struct CodeSuggestion {
    std::string text;
    std::string description;
    TokenType type;
};

/**
 * @brief Interaktiver Code-Editor mit Live-Tensor-Vorschau
 */
class CodeEditor {
public:
    CodeEditor();

    void update(float deltaTime);
    void render();

    // Code-Management
    void setText(const std::string& text);
    std::string getText() const;
    void clear();

    // Ausführung
    void execute();
    bool hasResult() const { return hasResult_; }
    const tensor::Tensor& getResult() const { return resultTensor_; }
    const std::string& getOutput() const { return output_; }

    // Vorlagen
    void loadTemplate(const std::string& name);
    static std::vector<std::pair<std::string, std::string>> getTemplates();

    // Einstellungen
    void setShowLineNumbers(bool show) { showLineNumbers_ = show; }
    void setFontSize(int size) { fontSize_ = size; }
    void setAutoExecute(bool enable) { autoExecute_ = enable; }

private:
    // Rendering
    void renderEditor();
    void renderLineNumbers();
    void renderCodeLines();
    void renderCursor();
    void renderSelection();
    void renderSuggestions();
    void renderOutput();
    void renderVisualization();
    void renderToolbar();

    // Input-Handling
    void handleKeyboard();
    void handleMouse();
    void handleTextInput();

    // Cursor-Bewegung
    void moveCursorLeft();
    void moveCursorRight();
    void moveCursorUp();
    void moveCursorDown();
    void moveCursorToLineStart();
    void moveCursorToLineEnd();
    void moveCursorWordLeft();
    void moveCursorWordRight();

    // Editing
    void insertChar(char c);
    void insertText(const std::string& text);
    void deleteChar();
    void backspaceChar();
    void insertNewLine();
    void deleteLine();

    // Selection
    void selectAll();
    void selectWord();
    void selectLine();
    void deleteSelection();
    std::string getSelectedText() const;
    bool hasSelection() const;

    // Copy/Paste
    void copy();
    void cut();
    void paste();

    // Undo/Redo
    void undo();
    void redo();
    void pushUndoState();

    // Syntax Highlighting
    void tokenizeLine(EditorLine& line);
    void tokenizeAllLines();
    Color getTokenColor(TokenType type) const;

    // Autovervollständigung
    void updateSuggestions();
    void applySuggestion(const CodeSuggestion& suggestion);
    void showSuggestions();
    void hideSuggestions();

    // Code-Interpretation (vereinfacht)
    void interpretCode();
    tensor::Tensor parseExpression(const std::string& expr);
    tensor::Tensor evaluateFunction(const std::string& func, const std::vector<std::string>& args);

    // Daten
    std::vector<EditorLine> lines_;
    int cursorLine_ = 0;
    int cursorCol_ = 0;
    int selectionStartLine_ = -1;
    int selectionStartCol_ = -1;

    // Scroll
    float scrollX_ = 0;
    float scrollY_ = 0;
    float targetScrollY_ = 0;

    // UI
    Rectangle editorBounds_;
    Rectangle vizBounds_;
    Rectangle outputBounds_;
    bool showLineNumbers_ = true;
    int fontSize_ = 16;
    float lineHeight_ = 22;
    float charWidth_ = 9.6f;

    // Cursor-Blink
    float cursorBlinkTimer_ = 0;
    bool cursorVisible_ = true;

    // Autovervollständigung
    bool showingSuggestions_ = false;
    std::vector<CodeSuggestion> suggestions_;
    int selectedSuggestion_ = 0;

    // Undo/Redo
    std::vector<std::string> undoStack_;
    std::vector<std::string> redoStack_;
    int maxUndoSize_ = 50;

    // Ausführung
    bool autoExecute_ = true;
    float executeDelay_ = 0.5f;
    float timeSinceLastEdit_ = 0;
    bool hasResult_ = false;
    tensor::Tensor resultTensor_;
    std::string output_;
    bool hasError_ = false;
    std::string errorMessage_;

    // Variablen-Speicher
    std::map<std::string, tensor::Tensor> variables_;

    // Visualisierung
    gui::TensorVisualizer visualizer_;

    // Keywords für Highlighting
    static const std::vector<std::string> keywords_;
    static const std::vector<std::string> types_;
    static const std::vector<std::string> tensorMethods_;
};

} // namespace editor
