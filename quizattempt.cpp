
#include <crow.h>
#include <crow/mustache.h>
#include <string>
#include <vector>


struct QuestionNode {
    std::string questionText;
    std::vector<std::string> options; 
    char correctAnswer; 
    QuestionNode* next;
    QuestionNode* prev;

    QuestionNode(const std::string& q, const std::vector<std::string>& opts, char ans)
        : questionText(q), options(opts), correctAnswer(ans), next(nullptr), prev(nullptr) {}
};

QuestionNode* quizHead = nullptr;

void addQuestion(const std::string& questionText, const std::vector<std::string>& options, char correctAnswer) {
    QuestionNode* newNode = new QuestionNode(questionText, options, correctAnswer);
    if (!quizHead) {
        quizHead = newNode;
        return;
    }
    QuestionNode* temp = quizHead;
    while (temp->next) temp = temp->next;
    temp->next = newNode;
    newNode->prev = temp;
}


void initializeQuiz() {
    addQuestion("What is the SI unit of force?", {"Joule", "Watt", "Newton", "Pascal"}, 'c');
    addQuestion("Which law of motion is also known as the law of inertia?", {"First Law", "Second Law", "Third Law", "Fourth Law"}, 'a');
}


struct QuizSubmissionNode {
    std::string studentName;
    std::vector<char> answers; 
    QuizSubmissionNode* next;
    QuizSubmissionNode* prev;

    QuizSubmissionNode(const std::string& name, const std::vector<char>& ans)
        : studentName(name), answers(ans), next(nullptr), prev(nullptr) {}
};

QuizSubmissionNode* submissionHead = nullptr;

void addSubmission(const std::string& studentName, const std::vector<char>& answers) {
    QuizSubmissionNode* newNode = new QuizSubmissionNode(studentName, answers);
    if (!submissionHead) {
        submissionHead = newNode;
        return;
    }
    QuizSubmissionNode* temp = submissionHead;
    while (temp->next) temp = temp->next;
    temp->next = newNode;
    newNode->prev = temp;
}


int main() {
    crow::SimpleApp app;

    initializeQuiz();


    CROW_ROUTE(app, "/static/<string>")
    ([](std::string filename){
        return crow::response(crow::mustache::load("static/" + filename).render());
    });

    
    CROW_ROUTE(app, "/quiz-attempt")
    ([](){
        crow::mustache::context ctx;
        std::vector<crow::mustache::context> questionsCtx;

        QuestionNode* temp = quizHead;
        int qIndex = 1;
        while (temp) {
            crow::mustache::context qctx;
            qctx["qindex"] = qIndex;
            qctx["questionText"] = temp->questionText;
            qctx["optionA"] = temp->options[0];
            qctx["optionB"] = temp->options[1];
            qctx["optionC"] = temp->options[2];
            qctx["optionD"] = temp->options[3];
            questionsCtx.push_back(qctx);

            temp = temp->next;
            qIndex++;
        }

        ctx["questions"] = questionsCtx;
        auto page = crow::mustache::load("quiz_attempt.html");
        return page.render(ctx);
    });

   
    CROW_ROUTE(app, "/submit-quiz").methods(crow::HTTPMethod::Post)
    ([](const crow::request& req){
        auto body = crow::query_string(req.body);
        std::vector<char> answers;

       
        int qIndex = 1;
        while (true) {
            std::string key = "q" + std::to_string(qIndex);
            const char* ans = body.get(key.c_str());
            if (!ans) break;
            answers.push_back(ans[0]);
            qIndex++;
        }

        std::string studentName = "Student1";
        addSubmission(studentName, answers);

        return crow::response("Quiz submitted successfully! You answered " + std::to_string(answers.size()) + " questions.");
    });

    app.port(18080).multithreaded().run();
}
