#include "editor/CodeEditor.hpp"
#include <algorithm>
#include <sstream>
#include <regex>

namespace editor {

// Static members
const std::vector<std::string> CodeEditor::keywords_ = {
    "auto", "const", "float", "int", "size_t", "void", "return",
    "if", "else", "for", "while", "break", "continue", "true", "false"
};

const std::vector<std::string> CodeEditor::types_ = {
    "Tensor", "TensorDB", "Shape", "DataType"
};

const std::vector<std::string> CodeEditor::tensorMethods_ = {
    "zeros", "ones", "random", "range", "identity", "fill",
    "reshape", "transpose", "flatten", "squeeze", "unsqueeze",
    "matmul", "dot", "sum", "mean", "min", "max",
    "sqrt", "exp", "log", "abs", "sin", "cos", "pow",
    "shape", "rank", "size", "at", "normalize", "fromVector", "fromMatrix"
};

CodeEditor::CodeEditor() {
    lines_.push_back({"", {}, false, ""});
    visualizer_.setShowGrid(false);
}

void CodeEditor::update(float deltaTime) {
    // Cursor blinken
    cursorBlinkTimer_ += deltaTime;
    if (cursorBlinkTimer_ >= 0.5f) {
        cursorBlinkTimer_ = 0;
        cursorVisible_ = !cursorVisible_;
    }

    // Smooth Scroll
    scrollY_ += (targetScrollY_ - scrollY_) * deltaTime * 10;

    // Auto-Execute
    if (autoExecute_) {
        timeSinceLastEdit_ += deltaTime;
        if (timeSinceLastEdit_ >= executeDelay_ && timeSinceLastEdit_ < executeDelay_ + deltaTime * 2) {
            execute();
        }
    }

    // Input
    handleKeyboard();
    handleMouse();
    handleTextInput();

    visualizer_.update(deltaTime);
}

void CodeEditor::render() {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    // Layout berechnen
    float toolbarHeight = 45;
    float outputHeight = 120;
    float vizWidth = 400;

    editorBounds_ = {20, 80 + toolbarHeight, static_cast<float>(screenWidth) - vizWidth - 60, static_cast<float>(screenHeight) - 180 - outputHeight};
    vizBounds_ = {editorBounds_.x + editorBounds_.width + 20, editorBounds_.y, vizWidth, editorBounds_.height};
    outputBounds_ = {20, editorBounds_.y + editorBounds_.height + 10, static_cast<float>(screenWidth) - 40, outputHeight};

    renderToolbar();
    renderEditor();
    renderVisualization();
    renderOutput();
}

void CodeEditor::renderToolbar() {
    float y = 80;
    float x = 20;
    int screenWidth = GetScreenWidth();

    // Hintergrund
    DrawRectangle(static_cast<int>(x), static_cast<int>(y), screenWidth - 40, 40, gui::Colors::BACKGROUND_LIGHT);

    // Buttons
    if (gui::UIComponents::button({x + 5, y + 5, 80, 30}, "Ausfuehren")) {
        execute();
    }

    if (gui::UIComponents::button({x + 95, y + 5, 60, 30}, "Leeren")) {
        clear();
    }

    // Templates Dropdown (vereinfacht als Buttons)
    DrawText("Vorlagen:", static_cast<int>(x + 180), static_cast<int>(y + 12), 14, gui::Colors::TEXT_DIM);

    auto templates = getTemplates();
    float templateX = x + 250;
    for (size_t i = 0; i < std::min(templates.size(), size_t(4)); ++i) {
        if (gui::UIComponents::button({templateX, y + 5, 100, 30}, templates[i].first)) {
            loadTemplate(templates[i].first);
        }
        templateX += 110;
    }

    // Auto-Execute Toggle
    float rightX = static_cast<float>(screenWidth) - 180;
    DrawText("Auto-Ausfuehren:", static_cast<int>(rightX), static_cast<int>(y + 12), 14, gui::Colors::TEXT_DIM);
    Rectangle toggleRect = {rightX + 110, y + 8, 50, 24};
    DrawRectangleRounded(toggleRect, 0.5f, 4, autoExecute_ ? gui::Colors::ACCENT_GREEN : gui::Colors::BUTTON);
    gui::UIComponents::labelCentered(toggleRect, autoExecute_ ? "AN" : "AUS", 12, gui::Colors::TEXT);
    if (CheckCollisionPointRec(GetMousePosition(), toggleRect) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        autoExecute_ = !autoExecute_;
    }
}

void CodeEditor::renderEditor() {
    // Hintergrund
    DrawRectangleRounded(editorBounds_, 0.01f, 4, {20, 20, 30, 255});
    DrawRectangleRoundedLines(editorBounds_, 0.01f, 4, 1, gui::Colors::BORDER);

    BeginScissorMode(static_cast<int>(editorBounds_.x), static_cast<int>(editorBounds_.y),
                     static_cast<int>(editorBounds_.width), static_cast<int>(editorBounds_.height));

    if (showLineNumbers_) {
        renderLineNumbers();
    }

    renderSelection();
    renderCodeLines();
    renderCursor();

    EndScissorMode();

    if (showingSuggestions_) {
        renderSuggestions();
    }
}

void CodeEditor::renderLineNumbers() {
    float lineNumWidth = 50;
    float startY = editorBounds_.y + 5 - scrollY_;

    DrawRectangle(static_cast<int>(editorBounds_.x), static_cast<int>(editorBounds_.y),
                  static_cast<int>(lineNumWidth), static_cast<int>(editorBounds_.height), {25, 25, 35, 255});

    for (size_t i = 0; i < lines_.size(); ++i) {
        float y = startY + i * lineHeight_;
        if (y < editorBounds_.y - lineHeight_ || y > editorBounds_.y + editorBounds_.height) continue;

        Color numColor = (static_cast<int>(i) == cursorLine_) ? gui::Colors::TEXT : gui::Colors::TEXT_DIM;
        if (lines_[i].hasError) numColor = gui::Colors::ACCENT;

        DrawText(TextFormat("%3d", static_cast<int>(i + 1)),
                 static_cast<int>(editorBounds_.x + 5), static_cast<int>(y), fontSize_, numColor);
    }
}

void CodeEditor::renderCodeLines() {
    float lineNumWidth = showLineNumbers_ ? 55 : 5;
    float startX = editorBounds_.x + lineNumWidth - scrollX_;
    float startY = editorBounds_.y + 5 - scrollY_;

    for (size_t i = 0; i < lines_.size(); ++i) {
        float y = startY + i * lineHeight_;
        if (y < editorBounds_.y - lineHeight_ || y > editorBounds_.y + editorBounds_.height) continue;

        // Fehler-Hintergrund
        if (lines_[i].hasError) {
            DrawRectangle(static_cast<int>(editorBounds_.x + lineNumWidth - 5), static_cast<int>(y - 2),
                          static_cast<int>(editorBounds_.width - lineNumWidth), static_cast<int>(lineHeight_),
                          {255, 100, 100, 30});
        }

        // Aktive Zeile
        if (static_cast<int>(i) == cursorLine_) {
            DrawRectangle(static_cast<int>(editorBounds_.x + lineNumWidth - 5), static_cast<int>(y - 2),
                          static_cast<int>(editorBounds_.width - lineNumWidth), static_cast<int>(lineHeight_),
                          {50, 50, 70, 100});
        }

        // Tokens zeichnen
        if (lines_[i].tokens.empty()) {
            DrawText(lines_[i].text.c_str(), static_cast<int>(startX), static_cast<int>(y), fontSize_, gui::Colors::TEXT);
        } else {
            for (const auto& token : lines_[i].tokens) {
                float x = startX + token.startCol * charWidth_;
                DrawText(token.text.c_str(), static_cast<int>(x), static_cast<int>(y), fontSize_, getTokenColor(token.type));
            }
        }
    }
}

void CodeEditor::renderCursor() {
    if (!cursorVisible_) return;

    float lineNumWidth = showLineNumbers_ ? 55 : 5;
    float x = editorBounds_.x + lineNumWidth + cursorCol_ * charWidth_ - scrollX_;
    float y = editorBounds_.y + 5 + cursorLine_ * lineHeight_ - scrollY_;

    if (x >= editorBounds_.x && x < editorBounds_.x + editorBounds_.width &&
        y >= editorBounds_.y && y < editorBounds_.y + editorBounds_.height) {
        DrawRectangle(static_cast<int>(x), static_cast<int>(y), 2, static_cast<int>(lineHeight_ - 4), gui::Colors::PRIMARY);
    }
}

void CodeEditor::renderSelection() {
    if (!hasSelection()) return;

    int startLine = std::min(selectionStartLine_, cursorLine_);
    int endLine = std::max(selectionStartLine_, cursorLine_);
    int startCol = (startLine == selectionStartLine_) ? selectionStartCol_ : cursorCol_;
    int endCol = (endLine == cursorLine_) ? cursorCol_ : selectionStartCol_;

    if (startLine == endLine && startCol > endCol) std::swap(startCol, endCol);

    float lineNumWidth = showLineNumbers_ ? 55 : 5;
    float baseX = editorBounds_.x + lineNumWidth - scrollX_;
    float baseY = editorBounds_.y + 5 - scrollY_;

    for (int line = startLine; line <= endLine; ++line) {
        int lineStart = (line == startLine) ? startCol : 0;
        int lineEnd = (line == endLine) ? endCol : static_cast<int>(lines_[line].text.size());

        float x = baseX + lineStart * charWidth_;
        float y = baseY + line * lineHeight_;
        float width = (lineEnd - lineStart) * charWidth_;

        DrawRectangle(static_cast<int>(x), static_cast<int>(y - 2), static_cast<int>(width), static_cast<int>(lineHeight_),
                      {100, 149, 237, 80});
    }
}

void CodeEditor::renderSuggestions() {
    if (suggestions_.empty()) return;

    float lineNumWidth = showLineNumbers_ ? 55 : 5;
    float x = editorBounds_.x + lineNumWidth + cursorCol_ * charWidth_ - scrollX_;
    float y = editorBounds_.y + 5 + (cursorLine_ + 1) * lineHeight_ - scrollY_;

    float width = 250;
    float itemHeight = 28;
    float height = std::min(static_cast<float>(suggestions_.size()), 6.0f) * itemHeight + 10;

    // Hintergrund
    DrawRectangleRounded({x, y, width, height}, 0.05f, 4, gui::Colors::BACKGROUND_PANEL);
    DrawRectangleRoundedLines({x, y, width, height}, 0.05f, 4, 1, gui::Colors::BORDER);

    float itemY = y + 5;
    for (size_t i = 0; i < std::min(suggestions_.size(), size_t(6)); ++i) {
        Rectangle itemRect = {x + 5, itemY, width - 10, itemHeight - 2};

        if (static_cast<int>(i) == selectedSuggestion_) {
            DrawRectangleRounded(itemRect, 0.2f, 4, gui::Colors::PRIMARY_DARK);
        }

        // Typ-Icon
        Color typeColor = getTokenColor(suggestions_[i].type);
        DrawRectangle(static_cast<int>(itemRect.x + 5), static_cast<int>(itemRect.y + 6), 4, 14, typeColor);

        // Text
        DrawText(suggestions_[i].text.c_str(), static_cast<int>(itemRect.x + 15), static_cast<int>(itemRect.y + 5), 14, gui::Colors::TEXT);

        itemY += itemHeight;
    }
}

void CodeEditor::renderOutput() {
    gui::UIComponents::panel(outputBounds_, "Ausgabe");

    float textX = outputBounds_.x + 15;
    float textY = outputBounds_.y + 50;

    if (hasError_) {
        DrawText("Fehler:", static_cast<int>(textX), static_cast<int>(textY), 16, gui::Colors::ACCENT);
        DrawText(errorMessage_.c_str(), static_cast<int>(textX + 70), static_cast<int>(textY), 16, gui::Colors::TEXT);
    } else if (!output_.empty()) {
        DrawText(output_.c_str(), static_cast<int>(textX), static_cast<int>(textY), 14, gui::Colors::TEXT);
    } else {
        DrawText("Schreibe Code und druecke 'Ausfuehren' oder warte auf Auto-Ausfuehrung...",
                 static_cast<int>(textX), static_cast<int>(textY), 14, gui::Colors::TEXT_DIM);
    }
}

void CodeEditor::renderVisualization() {
    DrawRectangleRounded(vizBounds_, 0.02f, 4, gui::Colors::BACKGROUND_PANEL);
    DrawRectangleRoundedLines(vizBounds_, 0.02f, 4, 1, gui::Colors::BORDER);

    DrawText("Live-Vorschau", static_cast<int>(vizBounds_.x + 10), static_cast<int>(vizBounds_.y + 10), 16, gui::Colors::TEXT_DIM);

    if (hasResult_) {
        BeginScissorMode(static_cast<int>(vizBounds_.x), static_cast<int>(vizBounds_.y + 30),
                         static_cast<int>(vizBounds_.width), static_cast<int>(vizBounds_.height - 30));
        visualizer_.render3D();
        EndScissorMode();

        // Tensor-Info
        DrawText(TextFormat("Shape: %s", resultTensor_.shapeString().c_str()),
                 static_cast<int>(vizBounds_.x + 10), static_cast<int>(vizBounds_.y + vizBounds_.height - 50), 14, gui::Colors::TEXT);
        DrawText(TextFormat("Elemente: %zu", resultTensor_.size()),
                 static_cast<int>(vizBounds_.x + 10), static_cast<int>(vizBounds_.y + vizBounds_.height - 30), 14, gui::Colors::TEXT_DIM);
    } else {
        DrawText("Kein Tensor", static_cast<int>(vizBounds_.x + vizBounds_.width / 2 - 40),
                 static_cast<int>(vizBounds_.y + vizBounds_.height / 2), 18, gui::Colors::TEXT_DIM);
    }
}

void CodeEditor::handleKeyboard() {
    // Navigation
    if (IsKeyPressed(KEY_LEFT)) {
        if (IsKeyDown(KEY_LEFT_CONTROL)) moveCursorWordLeft();
        else moveCursorLeft();
        if (!IsKeyDown(KEY_LEFT_SHIFT)) { selectionStartLine_ = -1; }
        else if (selectionStartLine_ == -1) { selectionStartLine_ = cursorLine_; selectionStartCol_ = cursorCol_; }
    }
    if (IsKeyPressed(KEY_RIGHT)) {
        if (IsKeyDown(KEY_LEFT_CONTROL)) moveCursorWordRight();
        else moveCursorRight();
        if (!IsKeyDown(KEY_LEFT_SHIFT)) { selectionStartLine_ = -1; }
        else if (selectionStartLine_ == -1) { selectionStartLine_ = cursorLine_; selectionStartCol_ = cursorCol_; }
    }
    if (IsKeyPressed(KEY_UP)) {
        if (showingSuggestions_) {
            selectedSuggestion_ = std::max(0, selectedSuggestion_ - 1);
        } else {
            moveCursorUp();
            if (!IsKeyDown(KEY_LEFT_SHIFT)) selectionStartLine_ = -1;
        }
    }
    if (IsKeyPressed(KEY_DOWN)) {
        if (showingSuggestions_) {
            selectedSuggestion_ = std::min(static_cast<int>(suggestions_.size()) - 1, selectedSuggestion_ + 1);
        } else {
            moveCursorDown();
            if (!IsKeyDown(KEY_LEFT_SHIFT)) selectionStartLine_ = -1;
        }
    }
    if (IsKeyPressed(KEY_HOME)) { moveCursorToLineStart(); selectionStartLine_ = -1; }
    if (IsKeyPressed(KEY_END)) { moveCursorToLineEnd(); selectionStartLine_ = -1; }

    // Editing
    if (IsKeyPressed(KEY_BACKSPACE)) { backspaceChar(); }
    if (IsKeyPressed(KEY_DELETE)) { deleteChar(); }
    if (IsKeyPressed(KEY_ENTER)) {
        if (showingSuggestions_ && !suggestions_.empty()) {
            applySuggestion(suggestions_[selectedSuggestion_]);
            hideSuggestions();
        } else {
            insertNewLine();
        }
    }
    if (IsKeyPressed(KEY_TAB)) {
        if (showingSuggestions_ && !suggestions_.empty()) {
            applySuggestion(suggestions_[selectedSuggestion_]);
            hideSuggestions();
        } else {
            insertText("    ");
        }
    }
    if (IsKeyPressed(KEY_ESCAPE)) { hideSuggestions(); selectionStartLine_ = -1; }

    // Shortcuts
    if (IsKeyDown(KEY_LEFT_CONTROL)) {
        if (IsKeyPressed(KEY_A)) selectAll();
        if (IsKeyPressed(KEY_C)) copy();
        if (IsKeyPressed(KEY_X)) cut();
        if (IsKeyPressed(KEY_V)) paste();
        if (IsKeyPressed(KEY_Z)) undo();
        if (IsKeyPressed(KEY_Y)) redo();
        if (IsKeyPressed(KEY_SPACE)) showSuggestions();
    }

    // F5 zum Ausführen
    if (IsKeyPressed(KEY_F5)) execute();
}

void CodeEditor::handleMouse() {
    Vector2 mouse = GetMousePosition();

    if (CheckCollisionPointRec(mouse, editorBounds_)) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            float lineNumWidth = showLineNumbers_ ? 55 : 5;
            int clickLine = static_cast<int>((mouse.y - editorBounds_.y - 5 + scrollY_) / lineHeight_);
            int clickCol = static_cast<int>((mouse.x - editorBounds_.x - lineNumWidth + scrollX_) / charWidth_);

            clickLine = std::clamp(clickLine, 0, static_cast<int>(lines_.size()) - 1);
            clickCol = std::clamp(clickCol, 0, static_cast<int>(lines_[clickLine].text.size()));

            cursorLine_ = clickLine;
            cursorCol_ = clickCol;
            selectionStartLine_ = -1;
            hideSuggestions();
        }

        // Scroll
        float wheel = GetMouseWheelMove();
        if (wheel != 0) {
            targetScrollY_ -= wheel * lineHeight_ * 3;
            targetScrollY_ = std::max(0.0f, targetScrollY_);
            float maxScroll = std::max(0.0f, lines_.size() * lineHeight_ - editorBounds_.height + 20);
            targetScrollY_ = std::min(targetScrollY_, maxScroll);
        }
    }
}

void CodeEditor::handleTextInput() {
    int key = GetCharPressed();
    while (key > 0) {
        if (key >= 32 && key <= 126) {
            if (hasSelection()) deleteSelection();
            insertChar(static_cast<char>(key));
        }
        key = GetCharPressed();
    }
}

void CodeEditor::moveCursorLeft() {
    if (cursorCol_ > 0) {
        cursorCol_--;
    } else if (cursorLine_ > 0) {
        cursorLine_--;
        cursorCol_ = static_cast<int>(lines_[cursorLine_].text.size());
    }
    cursorVisible_ = true;
    cursorBlinkTimer_ = 0;
}

void CodeEditor::moveCursorRight() {
    if (cursorCol_ < static_cast<int>(lines_[cursorLine_].text.size())) {
        cursorCol_++;
    } else if (cursorLine_ < static_cast<int>(lines_.size()) - 1) {
        cursorLine_++;
        cursorCol_ = 0;
    }
    cursorVisible_ = true;
    cursorBlinkTimer_ = 0;
}

void CodeEditor::moveCursorUp() {
    if (cursorLine_ > 0) {
        cursorLine_--;
        cursorCol_ = std::min(cursorCol_, static_cast<int>(lines_[cursorLine_].text.size()));
    }
    cursorVisible_ = true;
}

void CodeEditor::moveCursorDown() {
    if (cursorLine_ < static_cast<int>(lines_.size()) - 1) {
        cursorLine_++;
        cursorCol_ = std::min(cursorCol_, static_cast<int>(lines_[cursorLine_].text.size()));
    }
    cursorVisible_ = true;
}

void CodeEditor::moveCursorToLineStart() { cursorCol_ = 0; }
void CodeEditor::moveCursorToLineEnd() { cursorCol_ = static_cast<int>(lines_[cursorLine_].text.size()); }

void CodeEditor::moveCursorWordLeft() {
    while (cursorCol_ > 0 && !std::isalnum(lines_[cursorLine_].text[cursorCol_ - 1])) cursorCol_--;
    while (cursorCol_ > 0 && std::isalnum(lines_[cursorLine_].text[cursorCol_ - 1])) cursorCol_--;
}

void CodeEditor::moveCursorWordRight() {
    int len = static_cast<int>(lines_[cursorLine_].text.size());
    while (cursorCol_ < len && !std::isalnum(lines_[cursorLine_].text[cursorCol_])) cursorCol_++;
    while (cursorCol_ < len && std::isalnum(lines_[cursorLine_].text[cursorCol_])) cursorCol_++;
}

void CodeEditor::insertChar(char c) {
    pushUndoState();
    lines_[cursorLine_].text.insert(cursorCol_, 1, c);
    cursorCol_++;
    tokenizeLine(lines_[cursorLine_]);
    timeSinceLastEdit_ = 0;

    // Trigger suggestions für '.'
    if (c == '.') showSuggestions();
}

void CodeEditor::insertText(const std::string& text) {
    pushUndoState();
    lines_[cursorLine_].text.insert(cursorCol_, text);
    cursorCol_ += static_cast<int>(text.size());
    tokenizeLine(lines_[cursorLine_]);
    timeSinceLastEdit_ = 0;
}

void CodeEditor::deleteChar() {
    if (hasSelection()) {
        deleteSelection();
        return;
    }
    pushUndoState();
    if (cursorCol_ < static_cast<int>(lines_[cursorLine_].text.size())) {
        lines_[cursorLine_].text.erase(cursorCol_, 1);
    } else if (cursorLine_ < static_cast<int>(lines_.size()) - 1) {
        lines_[cursorLine_].text += lines_[cursorLine_ + 1].text;
        lines_.erase(lines_.begin() + cursorLine_ + 1);
    }
    tokenizeLine(lines_[cursorLine_]);
    timeSinceLastEdit_ = 0;
}

void CodeEditor::backspaceChar() {
    if (hasSelection()) {
        deleteSelection();
        return;
    }
    if (cursorCol_ > 0) {
        pushUndoState();
        cursorCol_--;
        lines_[cursorLine_].text.erase(cursorCol_, 1);
        tokenizeLine(lines_[cursorLine_]);
    } else if (cursorLine_ > 0) {
        pushUndoState();
        cursorCol_ = static_cast<int>(lines_[cursorLine_ - 1].text.size());
        lines_[cursorLine_ - 1].text += lines_[cursorLine_].text;
        lines_.erase(lines_.begin() + cursorLine_);
        cursorLine_--;
        tokenizeLine(lines_[cursorLine_]);
    }
    timeSinceLastEdit_ = 0;
}

void CodeEditor::insertNewLine() {
    pushUndoState();
    std::string remainder = lines_[cursorLine_].text.substr(cursorCol_);
    lines_[cursorLine_].text = lines_[cursorLine_].text.substr(0, cursorCol_);
    tokenizeLine(lines_[cursorLine_]);

    cursorLine_++;
    cursorCol_ = 0;
    lines_.insert(lines_.begin() + cursorLine_, {remainder, {}, false, ""});
    tokenizeLine(lines_[cursorLine_]);
    timeSinceLastEdit_ = 0;
}

void CodeEditor::selectAll() {
    selectionStartLine_ = 0;
    selectionStartCol_ = 0;
    cursorLine_ = static_cast<int>(lines_.size()) - 1;
    cursorCol_ = static_cast<int>(lines_.back().text.size());
}

bool CodeEditor::hasSelection() const {
    return selectionStartLine_ != -1 &&
           (selectionStartLine_ != cursorLine_ || selectionStartCol_ != cursorCol_);
}

void CodeEditor::deleteSelection() {
    if (!hasSelection()) return;
    pushUndoState();

    int startLine = std::min(selectionStartLine_, cursorLine_);
    int endLine = std::max(selectionStartLine_, cursorLine_);
    int startCol = (startLine == selectionStartLine_) ? selectionStartCol_ : cursorCol_;
    int endCol = (endLine == cursorLine_) ? cursorCol_ : selectionStartCol_;

    if (startLine == endLine && startCol > endCol) std::swap(startCol, endCol);

    if (startLine == endLine) {
        lines_[startLine].text.erase(startCol, endCol - startCol);
    } else {
        lines_[startLine].text = lines_[startLine].text.substr(0, startCol) + lines_[endLine].text.substr(endCol);
        lines_.erase(lines_.begin() + startLine + 1, lines_.begin() + endLine + 1);
    }

    cursorLine_ = startLine;
    cursorCol_ = startCol;
    selectionStartLine_ = -1;
    tokenizeLine(lines_[cursorLine_]);
    timeSinceLastEdit_ = 0;
}

void CodeEditor::copy() {
    if (hasSelection()) {
        SetClipboardText(getSelectedText().c_str());
    }
}

void CodeEditor::cut() {
    copy();
    deleteSelection();
}

void CodeEditor::paste() {
    const char* text = GetClipboardText();
    if (text) {
        if (hasSelection()) deleteSelection();
        insertText(text);
    }
}

std::string CodeEditor::getSelectedText() const {
    if (!hasSelection()) return "";

    int startLine = std::min(selectionStartLine_, cursorLine_);
    int endLine = std::max(selectionStartLine_, cursorLine_);
    int startCol = (startLine == selectionStartLine_) ? selectionStartCol_ : cursorCol_;
    int endCol = (endLine == cursorLine_) ? cursorCol_ : selectionStartCol_;

    if (startLine == endLine && startCol > endCol) std::swap(startCol, endCol);

    if (startLine == endLine) {
        return lines_[startLine].text.substr(startCol, endCol - startCol);
    }

    std::string result = lines_[startLine].text.substr(startCol) + "\n";
    for (int i = startLine + 1; i < endLine; ++i) {
        result += lines_[i].text + "\n";
    }
    result += lines_[endLine].text.substr(0, endCol);
    return result;
}

void CodeEditor::pushUndoState() {
    undoStack_.push_back(getText());
    if (undoStack_.size() > static_cast<size_t>(maxUndoSize_)) {
        undoStack_.erase(undoStack_.begin());
    }
    redoStack_.clear();
}

void CodeEditor::undo() {
    if (undoStack_.empty()) return;
    redoStack_.push_back(getText());
    setText(undoStack_.back());
    undoStack_.pop_back();
}

void CodeEditor::redo() {
    if (redoStack_.empty()) return;
    undoStack_.push_back(getText());
    setText(redoStack_.back());
    redoStack_.pop_back();
}

void CodeEditor::tokenizeLine(EditorLine& line) {
    line.tokens.clear();

    std::string& text = line.text;
    size_t i = 0;

    while (i < text.size()) {
        // Leerzeichen
        if (std::isspace(text[i])) {
            i++;
            continue;
        }

        Token token;
        token.startCol = static_cast<int>(i);

        // Kommentar
        if (i + 1 < text.size() && text[i] == '/' && text[i + 1] == '/') {
            token.text = text.substr(i);
            token.type = TokenType::Comment;
            token.endCol = static_cast<int>(text.size());
            line.tokens.push_back(token);
            break;
        }

        // String
        if (text[i] == '"') {
            size_t end = text.find('"', i + 1);
            if (end == std::string::npos) end = text.size();
            else end++;
            token.text = text.substr(i, end - i);
            token.type = TokenType::String;
            token.endCol = static_cast<int>(end);
            line.tokens.push_back(token);
            i = end;
            continue;
        }

        // Zahl
        if (std::isdigit(text[i]) || (text[i] == '.' && i + 1 < text.size() && std::isdigit(text[i + 1]))) {
            size_t start = i;
            while (i < text.size() && (std::isdigit(text[i]) || text[i] == '.' || text[i] == 'f')) i++;
            token.text = text.substr(start, i - start);
            token.type = TokenType::Number;
            token.endCol = static_cast<int>(i);
            line.tokens.push_back(token);
            continue;
        }

        // Identifier/Keyword
        if (std::isalpha(text[i]) || text[i] == '_') {
            size_t start = i;
            while (i < text.size() && (std::isalnum(text[i]) || text[i] == '_')) i++;
            token.text = text.substr(start, i - start);
            token.endCol = static_cast<int>(i);

            // Typ bestimmen
            if (std::find(keywords_.begin(), keywords_.end(), token.text) != keywords_.end()) {
                token.type = TokenType::Keyword;
            } else if (std::find(types_.begin(), types_.end(), token.text) != types_.end()) {
                token.type = TokenType::Type;
            } else if (std::find(tensorMethods_.begin(), tensorMethods_.end(), token.text) != tensorMethods_.end()) {
                token.type = TokenType::TensorMethod;
            } else {
                token.type = TokenType::Identifier;
            }

            line.tokens.push_back(token);
            continue;
        }

        // Operatoren
        if (text[i] == '+' || text[i] == '-' || text[i] == '*' || text[i] == '/' ||
            text[i] == '=' || text[i] == '<' || text[i] == '>' || text[i] == '!' ||
            text[i] == '&' || text[i] == '|' || text[i] == '^' || text[i] == '%') {
            token.text = std::string(1, text[i]);
            token.type = TokenType::Operator;
            token.endCol = static_cast<int>(i + 1);
            line.tokens.push_back(token);
            i++;
            continue;
        }

        // Klammern
        if (text[i] == '(' || text[i] == ')' || text[i] == '{' || text[i] == '}' ||
            text[i] == '[' || text[i] == ']') {
            token.text = std::string(1, text[i]);
            token.type = TokenType::Bracket;
            token.endCol = static_cast<int>(i + 1);
            line.tokens.push_back(token);
            i++;
            continue;
        }

        // Sonstige (Punkt, Komma, etc.)
        token.text = std::string(1, text[i]);
        token.type = TokenType::Unknown;
        token.endCol = static_cast<int>(i + 1);
        line.tokens.push_back(token);
        i++;
    }
}

void CodeEditor::tokenizeAllLines() {
    for (auto& line : lines_) {
        tokenizeLine(line);
    }
}

Color CodeEditor::getTokenColor(TokenType type) const {
    switch (type) {
        case TokenType::Keyword:      return {197, 134, 192, 255};  // Lila
        case TokenType::Type:         return {78, 201, 176, 255};   // Türkis
        case TokenType::Function:     return {220, 220, 170, 255};  // Gelb
        case TokenType::Number:       return {181, 206, 168, 255};  // Grün
        case TokenType::String:       return {206, 145, 120, 255};  // Orange
        case TokenType::Comment:      return {106, 153, 85, 255};   // Dunkelgrün
        case TokenType::Operator:     return gui::Colors::TEXT;
        case TokenType::TensorMethod: return {86, 156, 214, 255};   // Blau
        case TokenType::Bracket:      return {255, 215, 0, 255};    // Gold
        default:                      return gui::Colors::TEXT;
    }
}

void CodeEditor::showSuggestions() {
    updateSuggestions();
    if (!suggestions_.empty()) {
        showingSuggestions_ = true;
        selectedSuggestion_ = 0;
    }
}

void CodeEditor::hideSuggestions() {
    showingSuggestions_ = false;
}

void CodeEditor::updateSuggestions() {
    suggestions_.clear();

    // Finde das aktuelle Wort
    std::string& line = lines_[cursorLine_].text;
    int wordStart = cursorCol_;
    while (wordStart > 0 && (std::isalnum(line[wordStart - 1]) || line[wordStart - 1] == '_' || line[wordStart - 1] == '.')) {
        wordStart--;
    }
    std::string prefix = line.substr(wordStart, cursorCol_ - wordStart);

    // Nach '.' Tensor-Methoden vorschlagen
    if (!prefix.empty() && prefix.back() == '.') {
        for (const auto& method : tensorMethods_) {
            suggestions_.push_back({method, "Tensor Methode", TokenType::TensorMethod});
        }
        return;
    }

    // Prefix-Matching
    std::string lowerPrefix = prefix;
    std::transform(lowerPrefix.begin(), lowerPrefix.end(), lowerPrefix.begin(), ::tolower);

    for (const auto& kw : keywords_) {
        if (kw.find(lowerPrefix) == 0) {
            suggestions_.push_back({kw, "Keyword", TokenType::Keyword});
        }
    }
    for (const auto& type : types_) {
        if (type.find(prefix) == 0) {
            suggestions_.push_back({type, "Typ", TokenType::Type});
        }
    }
    for (const auto& method : tensorMethods_) {
        if (method.find(prefix) == 0) {
            suggestions_.push_back({method, "Tensor Methode", TokenType::TensorMethod});
        }
    }
}

void CodeEditor::applySuggestion(const CodeSuggestion& suggestion) {
    std::string& line = lines_[cursorLine_].text;
    int wordStart = cursorCol_;
    while (wordStart > 0 && (std::isalnum(line[wordStart - 1]) || line[wordStart - 1] == '_')) {
        wordStart--;
    }

    // Skip the '.' if present
    if (wordStart > 0 && line[wordStart - 1] == '.') {
        // Keep the dot
    }

    line = line.substr(0, wordStart) + suggestion.text + line.substr(cursorCol_);
    cursorCol_ = wordStart + static_cast<int>(suggestion.text.size());

    // Klammern hinzufügen für Funktionen
    if (suggestion.type == TokenType::TensorMethod) {
        line.insert(cursorCol_, "()");
        cursorCol_++;
    }

    tokenizeLine(lines_[cursorLine_]);
    timeSinceLastEdit_ = 0;
}

void CodeEditor::setText(const std::string& text) {
    lines_.clear();
    std::istringstream iss(text);
    std::string line;
    while (std::getline(iss, line)) {
        lines_.push_back({line, {}, false, ""});
    }
    if (lines_.empty()) lines_.push_back({"", {}, false, ""});

    cursorLine_ = 0;
    cursorCol_ = 0;
    tokenizeAllLines();
}

std::string CodeEditor::getText() const {
    std::string result;
    for (size_t i = 0; i < lines_.size(); ++i) {
        result += lines_[i].text;
        if (i < lines_.size() - 1) result += '\n';
    }
    return result;
}

void CodeEditor::clear() {
    setText("");
    output_.clear();
    hasResult_ = false;
    hasError_ = false;
    variables_.clear();
}

void CodeEditor::execute() {
    output_.clear();
    hasError_ = false;
    errorMessage_.clear();

    // Fehler zurücksetzen
    for (auto& line : lines_) {
        line.hasError = false;
        line.errorMessage.clear();
    }

    try {
        interpretCode();
    } catch (const std::exception& e) {
        hasError_ = true;
        errorMessage_ = e.what();
    }
}

void CodeEditor::interpretCode() {
    // Vereinfachter Interpreter für Tensor-Ausdrücke
    std::string code = getText();

    // Entferne Kommentare
    std::regex commentRegex("//.*");
    code = std::regex_replace(code, commentRegex, "");

    // Parse Zeile für Zeile
    std::istringstream iss(code);
    std::string line;
    int lineNum = 0;

    while (std::getline(iss, line)) {
        lineNum++;

        // Trim
        size_t start = line.find_first_not_of(" \t");
        if (start == std::string::npos) continue;
        line = line.substr(start);

        // Leere Zeilen überspringen
        if (line.empty()) continue;

        try {
            // Variablen-Zuweisung: auto x = ... oder Tensor x = ...
            std::regex assignRegex(R"((auto|Tensor)\s+(\w+)\s*=\s*(.+);?)");
            std::smatch match;

            if (std::regex_match(line, match, assignRegex)) {
                std::string varName = match[2];
                std::string expr = match[3];

                tensor::Tensor result = parseExpression(expr);
                variables_[varName] = result;

                output_ += varName + " = " + result.shapeString() + "\n";
                resultTensor_ = result;
                hasResult_ = true;
                visualizer_.setTensor(resultTensor_, varName);
            }
            // Direkte Expressions
            else if (!line.empty() && line[0] != '{' && line[0] != '}') {
                // Entferne trailing semicolon
                if (!line.empty() && line.back() == ';') line.pop_back();

                if (!line.empty()) {
                    tensor::Tensor result = parseExpression(line);
                    output_ += "Ergebnis: " + result.shapeString() + " - " + result.toString().substr(0, 50) + "\n";
                    resultTensor_ = result;
                    hasResult_ = true;
                    visualizer_.setTensor(resultTensor_, "result");
                }
            }
        } catch (const std::exception& e) {
            lines_[lineNum - 1].hasError = true;
            lines_[lineNum - 1].errorMessage = e.what();
            throw;
        }
    }
}

tensor::Tensor CodeEditor::parseExpression(const std::string& expr) {
    std::string e = expr;

    // Trim
    size_t start = e.find_first_not_of(" \t");
    size_t end = e.find_last_not_of(" \t;");
    if (start == std::string::npos) return tensor::Tensor();
    e = e.substr(start, end - start + 1);

    // Tensor::zeros({...})
    std::regex zerosRegex(R"(Tensor::zeros\(\{([^}]+)\}\))");
    std::smatch match;
    if (std::regex_search(e, match, zerosRegex)) {
        std::vector<size_t> shape;
        std::istringstream ss(match[1]);
        std::string dim;
        while (std::getline(ss, dim, ',')) {
            shape.push_back(std::stoul(dim));
        }
        return tensor::Tensor::zeros(shape);
    }

    // Tensor::ones({...})
    std::regex onesRegex(R"(Tensor::ones\(\{([^}]+)\}\))");
    if (std::regex_search(e, match, onesRegex)) {
        std::vector<size_t> shape;
        std::istringstream ss(match[1]);
        std::string dim;
        while (std::getline(ss, dim, ',')) {
            shape.push_back(std::stoul(dim));
        }
        return tensor::Tensor::ones(shape);
    }

    // Tensor::random({...})
    std::regex randomRegex(R"(Tensor::random\(\{([^}]+)\}\))");
    if (std::regex_search(e, match, randomRegex)) {
        std::vector<size_t> shape;
        std::istringstream ss(match[1]);
        std::string dim;
        while (std::getline(ss, dim, ',')) {
            shape.push_back(std::stoul(dim));
        }
        return tensor::Tensor::random(shape);
    }

    // Tensor::range(start, end)
    std::regex rangeRegex(R"(Tensor::range\(([^,]+),\s*([^)]+)\))");
    if (std::regex_search(e, match, rangeRegex)) {
        float start = std::stof(match[1]);
        float end = std::stof(match[2]);
        return tensor::Tensor::range(start, end);
    }

    // Tensor::identity(n)
    std::regex identityRegex(R"(Tensor::identity\((\d+)\))");
    if (std::regex_search(e, match, identityRegex)) {
        size_t n = std::stoul(match[1]);
        return tensor::Tensor::identity(n);
    }

    // Variable
    if (variables_.find(e) != variables_.end()) {
        return variables_[e];
    }

    // Methoden auf Variablen: var.transpose(), var.reshape({...})
    std::regex methodRegex(R"((\w+)\.(\w+)\((.*)\))");
    if (std::regex_search(e, match, methodRegex)) {
        std::string varName = match[1];
        std::string method = match[2];
        std::string args = match[3];

        if (variables_.find(varName) == variables_.end()) {
            throw std::runtime_error("Unbekannte Variable: " + varName);
        }

        tensor::Tensor t = variables_[varName];
        return evaluateFunction(method, {args});
    }

    throw std::runtime_error("Unbekannter Ausdruck: " + e);
}

tensor::Tensor CodeEditor::evaluateFunction(const std::string& func, const std::vector<std::string>& args) {
    // Hier würde die Funktion auf dem Tensor aufgerufen werden
    throw std::runtime_error("Funktion nicht implementiert: " + func);
}

void CodeEditor::loadTemplate(const std::string& name) {
    auto templates = getTemplates();
    for (const auto& [tname, code] : templates) {
        if (tname == name) {
            setText(code);
            execute();
            return;
        }
    }
}

std::vector<std::pair<std::string, std::string>> CodeEditor::getTemplates() {
    return {
        {"Basics", "// Tensor Grundlagen\nauto skalar = Tensor(42.0f);\nauto vektor = Tensor::range(1, 6);\nauto matrix = Tensor::identity(3);"},
        {"Random", "// Zufaellige Tensoren\nauto t1 = Tensor::random({4, 4});\nauto t2 = Tensor::random({3, 3, 3});"},
        {"Operationen", "// Tensor Operationen\nauto a = Tensor::ones({3, 3});\nauto b = Tensor::ones({3, 3}) * 2;\n// auto c = a + b;"},
        {"Neural", "// Neural Network Layer\nauto input = Tensor::random({1, 4});\nauto weights = Tensor::random({4, 3});\n// auto output = input.matmul(weights);"}
    };
}

} // namespace editor
