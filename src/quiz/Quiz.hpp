#pragma once

#include "raylib.h"
#include "gui/UIComponents.hpp"
#include "gui/Colors.hpp"
#include "tensor/Tensor.hpp"
#include <string>
#include <vector>
#include <functional>
#include <random>

namespace quiz {

/**
 * @brief Verschiedene Fragetypen
 */
enum class QuestionType {
    MultipleChoice,     // Mehrfachauswahl
    TensorShape,        // Shape erraten
    TensorValue,        // Wert berechnen
    CodeCompletion,     // Code vervollständigen
    TrueFalse,          // Wahr/Falsch
    Matching,           // Zuordnung
    Interactive         // Interaktive Tensor-Manipulation
};

/**
 * @brief Eine einzelne Frage
 */
struct Question {
    std::string id;
    QuestionType type;
    std::string question;
    std::string explanation;
    std::vector<std::string> options;
    std::vector<int> correctAnswers;  // Indizes der richtigen Antworten
    std::string codeSnippet;
    int difficulty;  // 1-5
    int points;

    // Für Tensor-Fragen
    std::function<tensor::Tensor()> tensorGenerator;
    std::string expectedAnswer;

    // Feedback
    std::string hintText;
    std::string successMessage;
    std::string failureMessage;
};

/**
 * @brief Ergebnis einer beantworteten Frage
 */
struct QuestionResult {
    std::string questionId;
    bool correct;
    int pointsEarned;
    float timeSpent;
    int attempts;
};

/**
 * @brief Ein komplettes Quiz
 */
struct Quiz {
    std::string id;
    std::string title;
    std::string description;
    std::vector<Question> questions;
    int passingScore;  // Mindestpunktzahl zum Bestehen
    float timeLimit;   // 0 = kein Limit
};

/**
 * @brief Quiz-Manager für interaktive Übungen
 */
class QuizManager {
public:
    QuizManager();

    // Quiz laden und starten
    void loadQuiz(const Quiz& quiz);
    void startQuiz();
    void resetQuiz();

    // Update und Render
    void update(float deltaTime);
    void render();

    // Navigation
    void nextQuestion();
    void previousQuestion();
    void submitAnswer();
    void useHint();

    // Antwort setzen
    void selectOption(int index);
    void setTextAnswer(const std::string& answer);
    void toggleOption(int index);  // Für Multiple-Select

    // Status
    bool isQuizComplete() const;
    bool isQuizPassed() const;
    int getCurrentScore() const { return currentScore_; }
    int getMaxScore() const;
    float getProgress() const;

    // Ergebnisse
    const std::vector<QuestionResult>& getResults() const { return results_; }

    // Vorgefertigte Quizze
    static Quiz createBasicsQuiz();
    static Quiz createCreationQuiz();
    static Quiz createOperationsQuiz();
    static Quiz createDatabaseQuiz();
    static Quiz createAdvancedQuiz();

private:
    void renderQuestion();
    void renderMultipleChoice();
    void renderTensorShape();
    void renderTensorValue();
    void renderCodeCompletion();
    void renderTrueFalse();
    void renderMatching();
    void renderInteractive();
    void renderFeedback();
    void renderProgress();
    void renderResults();

    bool checkAnswer();
    void recordResult(bool correct);

    Quiz currentQuiz_;
    int currentQuestionIndex_ = 0;
    std::vector<int> selectedOptions_;
    std::string textAnswer_;
    std::vector<QuestionResult> results_;

    int currentScore_ = 0;
    float questionTimer_ = 0.0f;
    int currentAttempts_ = 0;
    bool answerSubmitted_ = false;
    bool showingFeedback_ = false;
    bool showingResults_ = false;
    bool hintUsed_ = false;

    // Animation
    float feedbackAnimation_ = 0.0f;
    bool lastAnswerCorrect_ = false;

    // Tensor für interaktive Fragen
    tensor::Tensor interactiveTensor_;
    gui::TensorVisualizer questionVisualizer_;
};

/**
 * @brief Fortschrittsverfolgung über alle Quizze
 */
class QuizProgress {
public:
    struct QuizScore {
        std::string quizId;
        int bestScore;
        int maxScore;
        int attempts;
        bool passed;
        float bestTime;
    };

    void recordQuizResult(const std::string& quizId, int score, int maxScore, float time, bool passed);
    QuizScore getQuizScore(const std::string& quizId) const;
    std::vector<QuizScore> getAllScores() const;

    int getTotalPoints() const;
    float getOverallProgress() const;

    void saveProgress(const std::string& filename);
    void loadProgress(const std::string& filename);

private:
    std::map<std::string, QuizScore> scores_;
};

} // namespace quiz
