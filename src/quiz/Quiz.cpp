#include "quiz/Quiz.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>

namespace quiz {

QuizManager::QuizManager() {
    questionVisualizer_.setShowGrid(false);
    questionVisualizer_.setShowAxes(false);
}

void QuizManager::loadQuiz(const Quiz& quiz) {
    currentQuiz_ = quiz;
    resetQuiz();
}

void QuizManager::startQuiz() {
    resetQuiz();
}

void QuizManager::resetQuiz() {
    currentQuestionIndex_ = 0;
    selectedOptions_.clear();
    textAnswer_.clear();
    results_.clear();
    currentScore_ = 0;
    questionTimer_ = 0.0f;
    currentAttempts_ = 0;
    answerSubmitted_ = false;
    showingFeedback_ = false;
    showingResults_ = false;
    hintUsed_ = false;

    if (!currentQuiz_.questions.empty()) {
        auto& q = currentQuiz_.questions[0];
        if (q.tensorGenerator) {
            interactiveTensor_ = q.tensorGenerator();
            questionVisualizer_.setTensor(interactiveTensor_, "Quiz Tensor");
        }
    }
}

void QuizManager::update(float deltaTime) {
    questionTimer_ += deltaTime;

    if (showingFeedback_) {
        feedbackAnimation_ += deltaTime * 3.0f;
        if (feedbackAnimation_ > 1.0f) feedbackAnimation_ = 1.0f;
    }

    questionVisualizer_.update(deltaTime);
}

void QuizManager::render() {
    if (showingResults_) {
        renderResults();
        return;
    }

    renderProgress();
    renderQuestion();

    if (showingFeedback_) {
        renderFeedback();
    }
}

void QuizManager::nextQuestion() {
    if (!answerSubmitted_) {
        submitAnswer();
        return;
    }

    if (currentQuestionIndex_ < static_cast<int>(currentQuiz_.questions.size()) - 1) {
        currentQuestionIndex_++;
        selectedOptions_.clear();
        textAnswer_.clear();
        questionTimer_ = 0.0f;
        currentAttempts_ = 0;
        answerSubmitted_ = false;
        showingFeedback_ = false;
        hintUsed_ = false;
        feedbackAnimation_ = 0.0f;

        auto& q = currentQuiz_.questions[currentQuestionIndex_];
        if (q.tensorGenerator) {
            interactiveTensor_ = q.tensorGenerator();
            questionVisualizer_.setTensor(interactiveTensor_, "Quiz Tensor");
        }
    } else {
        showingResults_ = true;
    }
}

void QuizManager::previousQuestion() {
    if (currentQuestionIndex_ > 0 && !answerSubmitted_) {
        currentQuestionIndex_--;
        selectedOptions_.clear();
        textAnswer_.clear();

        auto& q = currentQuiz_.questions[currentQuestionIndex_];
        if (q.tensorGenerator) {
            interactiveTensor_ = q.tensorGenerator();
            questionVisualizer_.setTensor(interactiveTensor_, "Quiz Tensor");
        }
    }
}

void QuizManager::submitAnswer() {
    if (answerSubmitted_) return;

    bool correct = checkAnswer();
    recordResult(correct);
    answerSubmitted_ = true;
    showingFeedback_ = true;
    feedbackAnimation_ = 0.0f;
    lastAnswerCorrect_ = correct;
}

void QuizManager::useHint() {
    if (!hintUsed_) {
        hintUsed_ = true;
    }
}

void QuizManager::selectOption(int index) {
    if (answerSubmitted_) return;

    selectedOptions_.clear();
    selectedOptions_.push_back(index);
}

void QuizManager::setTextAnswer(const std::string& answer) {
    if (answerSubmitted_) return;
    textAnswer_ = answer;
}

void QuizManager::toggleOption(int index) {
    if (answerSubmitted_) return;

    auto it = std::find(selectedOptions_.begin(), selectedOptions_.end(), index);
    if (it != selectedOptions_.end()) {
        selectedOptions_.erase(it);
    } else {
        selectedOptions_.push_back(index);
    }
}

bool QuizManager::isQuizComplete() const {
    return showingResults_;
}

bool QuizManager::isQuizPassed() const {
    return currentScore_ >= currentQuiz_.passingScore;
}

int QuizManager::getMaxScore() const {
    int total = 0;
    for (const auto& q : currentQuiz_.questions) {
        total += q.points;
    }
    return total;
}

float QuizManager::getProgress() const {
    if (currentQuiz_.questions.empty()) return 0.0f;
    return static_cast<float>(currentQuestionIndex_ + 1) / currentQuiz_.questions.size();
}

bool QuizManager::checkAnswer() {
    if (currentQuestionIndex_ >= static_cast<int>(currentQuiz_.questions.size())) return false;

    auto& q = currentQuiz_.questions[currentQuestionIndex_];

    switch (q.type) {
        case QuestionType::MultipleChoice:
        case QuestionType::TrueFalse: {
            if (selectedOptions_.size() != q.correctAnswers.size()) return false;
            std::vector<int> sortedSelected = selectedOptions_;
            std::vector<int> sortedCorrect = q.correctAnswers;
            std::sort(sortedSelected.begin(), sortedSelected.end());
            std::sort(sortedCorrect.begin(), sortedCorrect.end());
            return sortedSelected == sortedCorrect;
        }

        case QuestionType::TensorShape:
        case QuestionType::TensorValue:
        case QuestionType::CodeCompletion: {
            // Normalisiere und vergleiche
            std::string normalizedAnswer = textAnswer_;
            std::string normalizedExpected = q.expectedAnswer;
            // Entferne Leerzeichen
            normalizedAnswer.erase(std::remove_if(normalizedAnswer.begin(), normalizedAnswer.end(), ::isspace), normalizedAnswer.end());
            normalizedExpected.erase(std::remove_if(normalizedExpected.begin(), normalizedExpected.end(), ::isspace), normalizedExpected.end());
            // Kleinschreibung
            std::transform(normalizedAnswer.begin(), normalizedAnswer.end(), normalizedAnswer.begin(), ::tolower);
            std::transform(normalizedExpected.begin(), normalizedExpected.end(), normalizedExpected.begin(), ::tolower);
            return normalizedAnswer == normalizedExpected;
        }

        case QuestionType::Matching: {
            return selectedOptions_ == q.correctAnswers;
        }

        default:
            return false;
    }
}

void QuizManager::recordResult(bool correct) {
    auto& q = currentQuiz_.questions[currentQuestionIndex_];

    QuestionResult result;
    result.questionId = q.id;
    result.correct = correct;
    result.timeSpent = questionTimer_;
    result.attempts = currentAttempts_ + 1;

    if (correct) {
        int points = q.points;
        if (hintUsed_) points = points / 2;  // Halbe Punkte bei Hinweis
        if (currentAttempts_ > 0) points = points / 2;  // Halbe Punkte bei Wiederholung
        result.pointsEarned = points;
        currentScore_ += points;
    } else {
        result.pointsEarned = 0;
    }

    results_.push_back(result);
}

void QuizManager::renderProgress() {
    int screenWidth = GetScreenWidth();

    // Fortschrittsleiste oben
    Rectangle progressBounds = {20, 80, static_cast<float>(screenWidth) - 40, 30};

    DrawRectangleRounded(progressBounds, 0.5f, 8, gui::Colors::BACKGROUND_LIGHT);

    float progress = getProgress();
    Rectangle fillBounds = {progressBounds.x + 2, progressBounds.y + 2,
                            (progressBounds.width - 4) * progress, progressBounds.height - 4};
    DrawRectangleRounded(fillBounds, 0.5f, 8, gui::Colors::PRIMARY);

    // Fragenummer und Punkte
    DrawText(TextFormat("Frage %d / %d", currentQuestionIndex_ + 1, (int)currentQuiz_.questions.size()),
             static_cast<int>(progressBounds.x + 10), static_cast<int>(progressBounds.y + 5), 18, gui::Colors::TEXT);

    std::string scoreText = TextFormat("Punkte: %d / %d", currentScore_, getMaxScore());
    int scoreWidth = MeasureText(scoreText.c_str(), 18);
    DrawText(scoreText.c_str(), static_cast<int>(progressBounds.x + progressBounds.width - scoreWidth - 10),
             static_cast<int>(progressBounds.y + 5), 18, gui::Colors::ACCENT_YELLOW);

    // Timer
    int minutes = static_cast<int>(questionTimer_) / 60;
    int seconds = static_cast<int>(questionTimer_) % 60;
    DrawText(TextFormat("%02d:%02d", minutes, seconds), screenWidth / 2 - 30,
             static_cast<int>(progressBounds.y + 5), 18, gui::Colors::TEXT_DIM);
}

void QuizManager::renderQuestion() {
    if (currentQuestionIndex_ >= static_cast<int>(currentQuiz_.questions.size())) return;

    auto& q = currentQuiz_.questions[currentQuestionIndex_];
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    // Frage-Panel
    float panelY = 130;
    float panelHeight = screenHeight - 200;

    Rectangle questionPanel = {20, panelY, static_cast<float>(screenWidth) - 40, panelHeight};
    gui::UIComponents::panel(questionPanel, "");

    // Schwierigkeitsindikator
    for (int i = 0; i < 5; ++i) {
        Color starColor = (i < q.difficulty) ? gui::Colors::ACCENT_YELLOW : gui::Colors::BACKGROUND_LIGHT;
        DrawText("*", static_cast<int>(questionPanel.x + questionPanel.width - 100 + i * 18),
                 static_cast<int>(questionPanel.y + 15), 20, starColor);
    }

    // Punkte
    DrawText(TextFormat("%d Punkte", q.points), static_cast<int>(questionPanel.x + 20),
             static_cast<int>(questionPanel.y + 15), 16, gui::Colors::ACCENT_GREEN);

    // Fragetext
    gui::UIComponents::paragraph({questionPanel.x + 20, questionPanel.y + 50, questionPanel.width - 40, 100},
                                  q.question, 22);

    // Code-Snippet wenn vorhanden
    float contentY = questionPanel.y + 160;
    if (!q.codeSnippet.empty()) {
        Rectangle codeRect = {questionPanel.x + 20, contentY, questionPanel.width / 2 - 40, 200};
        gui::UIComponents::codeBlock(codeRect, q.codeSnippet, "cpp");
        contentY += 220;
    }

    // Tensor-Visualisierung wenn vorhanden
    if (q.tensorGenerator) {
        Rectangle vizRect = {questionPanel.x + questionPanel.width / 2, questionPanel.y + 50,
                             questionPanel.width / 2 - 30, 250};
        DrawRectangleRounded(vizRect, 0.02f, 4, gui::Colors::BACKGROUND_LIGHT);

        BeginScissorMode(static_cast<int>(vizRect.x), static_cast<int>(vizRect.y),
                         static_cast<int>(vizRect.width), static_cast<int>(vizRect.height));
        questionVisualizer_.render3D();
        EndScissorMode();
    }

    // Typ-spezifisches Rendering
    float answerY = contentY;
    if (q.tensorGenerator) answerY = questionPanel.y + 320;

    switch (q.type) {
        case QuestionType::MultipleChoice:
            renderMultipleChoice();
            break;
        case QuestionType::TrueFalse:
            renderTrueFalse();
            break;
        case QuestionType::TensorShape:
        case QuestionType::TensorValue:
            renderTensorValue();
            break;
        case QuestionType::CodeCompletion:
            renderCodeCompletion();
            break;
        default:
            renderMultipleChoice();
            break;
    }

    // Hint-Button
    if (!hintUsed_ && !q.hintText.empty() && !answerSubmitted_) {
        if (gui::UIComponents::button({questionPanel.x + 20, questionPanel.y + panelHeight - 50, 120, 40}, "Hinweis")) {
            useHint();
        }
    }

    // Hinweis anzeigen
    if (hintUsed_ && !q.hintText.empty()) {
        DrawRectangle(static_cast<int>(questionPanel.x + 150), static_cast<int>(questionPanel.y + panelHeight - 55),
                      400, 50, {255, 215, 0, 30});
        DrawText(q.hintText.c_str(), static_cast<int>(questionPanel.x + 160),
                 static_cast<int>(questionPanel.y + panelHeight - 45), 16, gui::Colors::ACCENT_YELLOW);
    }

    // Submit/Next Button
    float buttonX = questionPanel.x + questionPanel.width - 150;
    float buttonY = questionPanel.y + panelHeight - 50;

    if (!answerSubmitted_) {
        if (gui::UIComponents::button({buttonX, buttonY, 130, 40}, "Antworten")) {
            submitAnswer();
        }
    } else {
        std::string btnText = (currentQuestionIndex_ < static_cast<int>(currentQuiz_.questions.size()) - 1)
                              ? "Weiter >" : "Ergebnisse";
        if (gui::UIComponents::button({buttonX, buttonY, 130, 40}, btnText)) {
            nextQuestion();
        }
    }
}

void QuizManager::renderMultipleChoice() {
    if (currentQuestionIndex_ >= static_cast<int>(currentQuiz_.questions.size())) return;

    auto& q = currentQuiz_.questions[currentQuestionIndex_];
    int screenWidth = GetScreenWidth();

    float optionY = 350;
    float optionWidth = 400;
    float optionX = (screenWidth - optionWidth) / 2;

    for (size_t i = 0; i < q.options.size(); ++i) {
        Rectangle optionRect = {optionX, optionY + i * 55, optionWidth, 50};

        bool selected = std::find(selectedOptions_.begin(), selectedOptions_.end(), static_cast<int>(i)) != selectedOptions_.end();
        bool isCorrect = std::find(q.correctAnswers.begin(), q.correctAnswers.end(), static_cast<int>(i)) != q.correctAnswers.end();

        Color bgColor = gui::Colors::BUTTON;
        Color borderColor = gui::Colors::BORDER;

        if (answerSubmitted_) {
            if (isCorrect) {
                bgColor = {80, 200, 120, 100};
                borderColor = gui::Colors::ACCENT_GREEN;
            } else if (selected && !isCorrect) {
                bgColor = {255, 107, 107, 100};
                borderColor = gui::Colors::ACCENT;
            }
        } else {
            if (selected) {
                bgColor = gui::Colors::PRIMARY_DARK;
                borderColor = gui::Colors::PRIMARY;
            } else if (CheckCollisionPointRec(GetMousePosition(), optionRect)) {
                bgColor = gui::Colors::BUTTON_HOVER;
            }
        }

        DrawRectangleRounded(optionRect, 0.2f, 4, bgColor);
        DrawRectangleRoundedLines(optionRect, 0.2f, 4, 2, borderColor);

        // Option-Buchstabe
        char letter = 'A' + static_cast<char>(i);
        DrawText(TextFormat("%c)", letter), static_cast<int>(optionRect.x + 15),
                 static_cast<int>(optionRect.y + 15), 20, gui::Colors::PRIMARY_LIGHT);

        // Option-Text
        DrawText(q.options[i].c_str(), static_cast<int>(optionRect.x + 50),
                 static_cast<int>(optionRect.y + 15), 18, gui::Colors::TEXT);

        // Klick-Handler
        if (!answerSubmitted_ && CheckCollisionPointRec(GetMousePosition(), optionRect) &&
            IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            selectOption(static_cast<int>(i));
        }
    }
}

void QuizManager::renderTrueFalse() {
    if (currentQuestionIndex_ >= static_cast<int>(currentQuiz_.questions.size())) return;

    auto& q = currentQuiz_.questions[currentQuestionIndex_];
    int screenWidth = GetScreenWidth();

    float buttonWidth = 200;
    float buttonHeight = 60;
    float spacing = 50;
    float startX = (screenWidth - 2 * buttonWidth - spacing) / 2;
    float buttonY = 400;

    // Wahr-Button
    Rectangle trueRect = {startX, buttonY, buttonWidth, buttonHeight};
    bool trueSelected = std::find(selectedOptions_.begin(), selectedOptions_.end(), 0) != selectedOptions_.end();
    bool trueIsCorrect = std::find(q.correctAnswers.begin(), q.correctAnswers.end(), 0) != q.correctAnswers.end();

    Color trueBg = gui::Colors::BUTTON;
    if (answerSubmitted_) {
        trueBg = trueIsCorrect ? Color{80, 200, 120, 150} : (trueSelected ? Color{255, 107, 107, 150} : gui::Colors::BUTTON);
    } else if (trueSelected) {
        trueBg = gui::Colors::ACCENT_GREEN;
    }

    DrawRectangleRounded(trueRect, 0.3f, 4, trueBg);
    gui::UIComponents::labelCentered(trueRect, "WAHR", 24, gui::Colors::TEXT_HIGHLIGHT);

    if (!answerSubmitted_ && CheckCollisionPointRec(GetMousePosition(), trueRect) &&
        IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        selectOption(0);
    }

    // Falsch-Button
    Rectangle falseRect = {startX + buttonWidth + spacing, buttonY, buttonWidth, buttonHeight};
    bool falseSelected = std::find(selectedOptions_.begin(), selectedOptions_.end(), 1) != selectedOptions_.end();
    bool falseIsCorrect = std::find(q.correctAnswers.begin(), q.correctAnswers.end(), 1) != q.correctAnswers.end();

    Color falseBg = gui::Colors::BUTTON;
    if (answerSubmitted_) {
        falseBg = falseIsCorrect ? Color{80, 200, 120, 150} : (falseSelected ? Color{255, 107, 107, 150} : gui::Colors::BUTTON);
    } else if (falseSelected) {
        falseBg = gui::Colors::ACCENT;
    }

    DrawRectangleRounded(falseRect, 0.3f, 4, falseBg);
    gui::UIComponents::labelCentered(falseRect, "FALSCH", 24, gui::Colors::TEXT_HIGHLIGHT);

    if (!answerSubmitted_ && CheckCollisionPointRec(GetMousePosition(), falseRect) &&
        IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        selectOption(1);
    }
}

void QuizManager::renderTensorValue() {
    int screenWidth = GetScreenWidth();

    float inputY = 400;
    float inputWidth = 300;
    float inputX = (screenWidth - inputWidth) / 2;

    Rectangle inputRect = {inputX, inputY, inputWidth, 45};

    if (answerSubmitted_) {
        auto& q = currentQuiz_.questions[currentQuestionIndex_];
        Color bgColor = lastAnswerCorrect_ ? Color{80, 200, 120, 100} : Color{255, 107, 107, 100};
        DrawRectangleRounded(inputRect, 0.2f, 4, bgColor);
        DrawText(textAnswer_.c_str(), static_cast<int>(inputRect.x + 15),
                 static_cast<int>(inputRect.y + 12), 20, gui::Colors::TEXT);

        // Richtige Antwort zeigen
        if (!lastAnswerCorrect_) {
            DrawText(TextFormat("Richtig: %s", q.expectedAnswer.c_str()),
                     static_cast<int>(inputX), static_cast<int>(inputY + 60), 18, gui::Colors::ACCENT_GREEN);
        }
    } else {
        gui::UIComponents::textInput(inputRect, textAnswer_, "Deine Antwort...");
    }
}

void QuizManager::renderCodeCompletion() {
    renderTensorValue();  // Ähnlich wie TensorValue
}

void QuizManager::renderFeedback() {
    if (!showingFeedback_) return;

    auto& q = currentQuiz_.questions[currentQuestionIndex_];
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    // Feedback-Overlay
    float alpha = feedbackAnimation_ * 230;
    DrawRectangle(0, 0, screenWidth, screenHeight, {0, 0, 0, static_cast<unsigned char>(alpha * 0.3f)});

    // Feedback-Box
    float boxWidth = 500;
    float boxHeight = 200;
    float boxX = (screenWidth - boxWidth) / 2;
    float boxY = (screenHeight - boxHeight) / 2;

    Color boxColor = lastAnswerCorrect_ ? Color{80, 200, 120, 240} : Color{255, 107, 107, 240};
    DrawRectangleRounded({boxX, boxY, boxWidth, boxHeight}, 0.1f, 4, boxColor);

    // Icon
    const char* icon = lastAnswerCorrect_ ? "+" : "X";
    DrawText(icon, static_cast<int>(boxX + 30), static_cast<int>(boxY + 30), 60, gui::Colors::TEXT_HIGHLIGHT);

    // Nachricht
    std::string message = lastAnswerCorrect_ ? q.successMessage : q.failureMessage;
    if (message.empty()) {
        message = lastAnswerCorrect_ ? "Richtig!" : "Leider falsch.";
    }
    DrawText(message.c_str(), static_cast<int>(boxX + 110), static_cast<int>(boxY + 40), 28, gui::Colors::TEXT_HIGHLIGHT);

    // Punkte
    auto& result = results_.back();
    if (lastAnswerCorrect_) {
        DrawText(TextFormat("+%d Punkte", result.pointsEarned),
                 static_cast<int>(boxX + 110), static_cast<int>(boxY + 80), 22, gui::Colors::TEXT);
    }

    // Erklärung
    if (!q.explanation.empty()) {
        gui::UIComponents::paragraph({boxX + 20, boxY + 120, boxWidth - 40, 60}, q.explanation, 16);
    }
}

void QuizManager::renderResults() {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    // Ergebnis-Panel
    float panelWidth = 600;
    float panelHeight = 500;
    float panelX = (screenWidth - panelWidth) / 2;
    float panelY = (screenHeight - panelHeight) / 2;

    gui::UIComponents::panel({panelX, panelY, panelWidth, panelHeight}, "Quiz Ergebnisse");

    // Bestanden/Nicht bestanden
    bool passed = isQuizPassed();
    Color resultColor = passed ? gui::Colors::ACCENT_GREEN : gui::Colors::ACCENT;
    const char* resultText = passed ? "BESTANDEN!" : "NICHT BESTANDEN";

    int textWidth = MeasureText(resultText, 36);
    DrawText(resultText, static_cast<int>(panelX + (panelWidth - textWidth) / 2),
             static_cast<int>(panelY + 60), 36, resultColor);

    // Punktzahl
    float scoreY = panelY + 120;
    DrawText(TextFormat("Punkte: %d / %d", currentScore_, getMaxScore()),
             static_cast<int>(panelX + 30), static_cast<int>(scoreY), 24, gui::Colors::TEXT);

    // Prozent
    float percentage = static_cast<float>(currentScore_) / getMaxScore() * 100;
    DrawText(TextFormat("%.1f%%", percentage),
             static_cast<int>(panelX + panelWidth - 100), static_cast<int>(scoreY), 24, gui::Colors::PRIMARY_LIGHT);

    // Fortschrittsbalken
    Rectangle progressRect = {panelX + 30, scoreY + 40, panelWidth - 60, 20};
    DrawRectangleRounded(progressRect, 0.5f, 4, gui::Colors::BACKGROUND_LIGHT);
    Rectangle fillRect = {progressRect.x, progressRect.y, progressRect.width * (percentage / 100.0f), progressRect.height};
    DrawRectangleRounded(fillRect, 0.5f, 4, resultColor);

    // Detaillierte Ergebnisse
    float detailY = scoreY + 90;
    DrawText("Detaillierte Ergebnisse:", static_cast<int>(panelX + 30), static_cast<int>(detailY), 20, gui::Colors::TEXT);
    detailY += 35;

    int correct = 0;
    for (const auto& r : results_) {
        if (r.correct) correct++;
    }

    DrawText(TextFormat("Richtige Antworten: %d / %d", correct, (int)results_.size()),
             static_cast<int>(panelX + 30), static_cast<int>(detailY), 18, gui::Colors::TEXT_DIM);
    detailY += 30;

    float totalTime = 0;
    for (const auto& r : results_) totalTime += r.timeSpent;
    int minutes = static_cast<int>(totalTime) / 60;
    int seconds = static_cast<int>(totalTime) % 60;
    DrawText(TextFormat("Gesamtzeit: %02d:%02d", minutes, seconds),
             static_cast<int>(panelX + 30), static_cast<int>(detailY), 18, gui::Colors::TEXT_DIM);

    // Buttons
    if (gui::UIComponents::button({panelX + 30, panelY + panelHeight - 70, 150, 45}, "Wiederholen")) {
        resetQuiz();
    }

    if (gui::UIComponents::button({panelX + panelWidth - 180, panelY + panelHeight - 70, 150, 45}, "Beenden")) {
        // Wird von außen gehandhabt
    }
}

// === Vorgefertigte Quizze ===

Quiz QuizManager::createBasicsQuiz() {
    Quiz quiz;
    quiz.id = "basics";
    quiz.title = "Tensor Grundlagen Quiz";
    quiz.description = "Teste dein Wissen über die Grundlagen von Tensoren";
    quiz.passingScore = 60;
    quiz.timeLimit = 0;

    // Frage 1
    quiz.questions.push_back({
        "b1", QuestionType::MultipleChoice,
        "Was ist der Rang eines Skalars?",
        "Ein Skalar ist ein einzelner Wert ohne Dimensionen, daher Rang 0.",
        {"0", "1", "2", "Undefiniert"},
        {0}, "", 1, 10, nullptr, "", "Denke an die Anzahl der Dimensionen", "Richtig!", "Ein Skalar hat keine Dimensionen."
    });

    // Frage 2
    quiz.questions.push_back({
        "b2", QuestionType::MultipleChoice,
        "Welche Shape hat ein Vektor mit 5 Elementen?",
        "Ein Vektor ist eindimensional, die Shape gibt die Anzahl der Elemente an.",
        {"(5)", "(1, 5)", "(5, 1)", "5"},
        {0}, "", 1, 10, nullptr, "", "", "Korrekt!", "Die Shape eines Vektors ist (n)."
    });

    // Frage 3
    quiz.questions.push_back({
        "b3", QuestionType::TrueFalse,
        "Eine 3x4 Matrix hat 12 Elemente.",
        "3 Zeilen × 4 Spalten = 12 Elemente",
        {"Wahr", "Falsch"},
        {0}, "", 1, 10, nullptr, "", "", "Genau!", ""
    });

    // Frage 4
    quiz.questions.push_back({
        "b4", QuestionType::TensorShape,
        "Welche Shape hat dieser Tensor?",
        "Zähle die Dimensionen und ihre Größen.",
        {}, {}, "", 2, 15,
        []() { return tensor::Tensor::random({2, 3, 4}); },
        "(2,3,4)", "Schaue auf die 3D-Visualisierung", "Perfekt!", "Die Shape ist (2, 3, 4)."
    });

    // Frage 5
    quiz.questions.push_back({
        "b5", QuestionType::MultipleChoice,
        "Was beschreibt 'Strides' bei einem Tensor?",
        "Strides geben an, wie viele Elemente im Speicher übersprungen werden.",
        {"Die Größe jeder Dimension", "Die Schrittweite im Speicher", "Die Anzahl der Elemente", "Den Datentyp"},
        {1}, "", 3, 15, nullptr, "", "", "Richtig!", ""
    });

    return quiz;
}

Quiz QuizManager::createCreationQuiz() {
    Quiz quiz;
    quiz.id = "creation";
    quiz.title = "Tensor Erstellung Quiz";
    quiz.description = "Teste dein Wissen über das Erstellen von Tensoren";
    quiz.passingScore = 50;

    quiz.questions.push_back({
        "c1", QuestionType::CodeCompletion,
        "Wie erstellt man einen Tensor voller Einsen mit Shape (3, 3)?",
        "Die ones() Funktion erstellt einen Tensor mit Einsen.",
        {}, {}, "Tensor t = Tensor::_____({3, 3});", 1, 10,
        nullptr, "ones", "", "Korrekt!", "Tensor::ones({3,3})"
    });

    quiz.questions.push_back({
        "c2", QuestionType::MultipleChoice,
        "Welche Methode erstellt Zufallswerte?",
        "",
        {"zeros()", "ones()", "random()", "fill()"},
        {2}, "", 1, 10, nullptr, "", "", "Richtig!", ""
    });

    quiz.questions.push_back({
        "c3", QuestionType::TensorValue,
        "Was ist der Wert an Position [1] bei Tensor::range(0, 5)?",
        "range(0, 5) erzeugt [0, 1, 2, 3, 4]",
        {}, {}, "", 2, 15,
        []() { return tensor::Tensor::range(0, 5); },
        "1", "Indizierung beginnt bei 0", "Genau!", "Bei Index 1 steht der Wert 1."
    });

    return quiz;
}

Quiz QuizManager::createOperationsQuiz() {
    Quiz quiz;
    quiz.id = "operations";
    quiz.title = "Tensor Operationen Quiz";
    quiz.description = "Teste dein Wissen über Tensor-Berechnungen";
    quiz.passingScore = 50;

    quiz.questions.push_back({
        "o1", QuestionType::TrueFalse,
        "Der * Operator führt Matrixmultiplikation durch.",
        "Der * Operator ist elementweise, matmul() ist Matrixmultiplikation.",
        {"Wahr", "Falsch"},
        {1}, "", 2, 15, nullptr, "", "", "Korrekt!", "* ist elementweise, matmul() für Matrizen."
    });

    quiz.questions.push_back({
        "o2", QuestionType::TensorValue,
        "Was ist das Ergebnis von sum() für diesen Tensor?",
        "",
        {}, {}, "", 2, 15,
        []() { return tensor::Tensor::fromVector({1, 2, 3, 4, 5}); },
        "15", "Addiere alle Elemente", "Richtig!", "1+2+3+4+5 = 15"
    });

    quiz.questions.push_back({
        "o3", QuestionType::MultipleChoice,
        "Was passiert bei reshape({6}) auf eine (2,3) Matrix?",
        "reshape ändert nur die Form, nicht die Daten.",
        {"Fehler - inkompatible Größen", "Ein Vektor mit 6 Elementen", "Eine 6x1 Matrix", "Die Werte werden verdoppelt"},
        {1}, "", 2, 15, nullptr, "", "", "Genau!", "2*3 = 6 Elemente bleiben erhalten."
    });

    return quiz;
}

Quiz QuizManager::createDatabaseQuiz() {
    Quiz quiz;
    quiz.id = "database";
    quiz.title = "Tensor Datenbank Quiz";
    quiz.description = "Teste dein Wissen über Tensor-Datenbanken";
    quiz.passingScore = 50;

    quiz.questions.push_back({
        "d1", QuestionType::MultipleChoice,
        "Welche Methode speichert einen Tensor in der DB?",
        "",
        {"save()", "store()", "put()", "insert()"},
        {1}, "", 1, 10, nullptr, "", "", "Richtig!", ""
    });

    quiz.questions.push_back({
        "d2", QuestionType::TrueFalse,
        "TensorDB kann Tensoren mit Tags versehen.",
        "setTag() und findByTag() ermöglichen Tag-basierte Organisation.",
        {"Wahr", "Falsch"},
        {0}, "", 1, 10, nullptr, "", "", "Korrekt!", ""
    });

    return quiz;
}

Quiz QuizManager::createAdvancedQuiz() {
    Quiz quiz;
    quiz.id = "advanced";
    quiz.title = "Fortgeschrittenes Quiz";
    quiz.description = "Herausfordernde Fragen für Experten";
    quiz.passingScore = 70;

    quiz.questions.push_back({
        "a1", QuestionType::TensorShape,
        "Welche Shape hat das Ergebnis von matmul auf (2,3) und (3,4)?",
        "Bei Matrixmultiplikation: (m,n) @ (n,p) = (m,p)",
        {}, {}, "", 4, 20, nullptr, "(2,4)", "", "Perfekt!", ""
    });

    quiz.questions.push_back({
        "a2", QuestionType::TensorValue,
        "Was ist die Norm dieses Vektors (gerundet auf 2 Stellen)?",
        "Die euklidische Norm ist sqrt(sum(x^2))",
        {}, {}, "", 4, 20,
        []() { return tensor::Tensor::fromVector({3, 4}); },
        "5", "sqrt(3² + 4²)", "Korrekt!", "sqrt(9 + 16) = sqrt(25) = 5"
    });

    return quiz;
}

// === QuizProgress ===

void QuizProgress::recordQuizResult(const std::string& quizId, int score, int maxScore, float time, bool passed) {
    auto& entry = scores_[quizId];
    entry.quizId = quizId;
    entry.attempts++;
    entry.maxScore = maxScore;
    entry.passed = entry.passed || passed;

    if (score > entry.bestScore) {
        entry.bestScore = score;
    }
    if (entry.bestTime == 0 || time < entry.bestTime) {
        entry.bestTime = time;
    }
}

QuizProgress::QuizScore QuizProgress::getQuizScore(const std::string& quizId) const {
    auto it = scores_.find(quizId);
    if (it != scores_.end()) {
        return it->second;
    }
    return {};
}

std::vector<QuizProgress::QuizScore> QuizProgress::getAllScores() const {
    std::vector<QuizScore> result;
    for (const auto& [id, score] : scores_) {
        result.push_back(score);
    }
    return result;
}

int QuizProgress::getTotalPoints() const {
    int total = 0;
    for (const auto& [id, score] : scores_) {
        total += score.bestScore;
    }
    return total;
}

float QuizProgress::getOverallProgress() const {
    if (scores_.empty()) return 0.0f;
    int passed = 0;
    for (const auto& [id, score] : scores_) {
        if (score.passed) passed++;
    }
    return static_cast<float>(passed) / scores_.size();
}

void QuizProgress::saveProgress(const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) return;

    size_t count = scores_.size();
    file.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (const auto& [id, score] : scores_) {
        size_t idLen = id.size();
        file.write(reinterpret_cast<const char*>(&idLen), sizeof(idLen));
        file.write(id.data(), idLen);
        file.write(reinterpret_cast<const char*>(&score.bestScore), sizeof(score.bestScore));
        file.write(reinterpret_cast<const char*>(&score.maxScore), sizeof(score.maxScore));
        file.write(reinterpret_cast<const char*>(&score.attempts), sizeof(score.attempts));
        file.write(reinterpret_cast<const char*>(&score.passed), sizeof(score.passed));
        file.write(reinterpret_cast<const char*>(&score.bestTime), sizeof(score.bestTime));
    }
}

void QuizProgress::loadProgress(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) return;

    scores_.clear();

    size_t count;
    file.read(reinterpret_cast<char*>(&count), sizeof(count));

    for (size_t i = 0; i < count; ++i) {
        size_t idLen;
        file.read(reinterpret_cast<char*>(&idLen), sizeof(idLen));

        std::string id(idLen, '\0');
        file.read(id.data(), idLen);

        QuizScore score;
        score.quizId = id;
        file.read(reinterpret_cast<char*>(&score.bestScore), sizeof(score.bestScore));
        file.read(reinterpret_cast<char*>(&score.maxScore), sizeof(score.maxScore));
        file.read(reinterpret_cast<char*>(&score.attempts), sizeof(score.attempts));
        file.read(reinterpret_cast<char*>(&score.passed), sizeof(score.passed));
        file.read(reinterpret_cast<char*>(&score.bestTime), sizeof(score.bestTime));

        scores_[id] = score;
    }
}

} // namespace quiz
