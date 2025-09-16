
#include <crow.h>
#include <crow/mustache.h>
#include <string>
#include <vector>


struct ProgressNode {
    std::string username;
    int averageScore;         
    int quizzesCompleted;
    std::string bestSubject;
    ProgressNode* next;
    ProgressNode* prev;

    ProgressNode(const std::string& u, int avg, int quizzes, const std::string& subject)
        : username(u), averageScore(avg), quizzesCompleted(quizzes), bestSubject(subject),
          next(nullptr), prev(nullptr) {}
};

ProgressNode* progressHead = nullptr;

void addProgress(const std::string& username, int avg, int quizzes, const std::string& subject) {
    ProgressNode* newNode = new ProgressNode(username, avg, quizzes, subject);
    if (!progressHead) {
        progressHead = newNode;
        return;
    }
    ProgressNode* temp = progressHead;
    while (temp->next) temp = temp->next;
    temp->next = newNode;
    newNode->prev = temp;
}

ProgressNode* findProgress(const std::string& username) {
    ProgressNode* temp = progressHead;
    while (temp) {
        if (temp->username == username) return temp;
        temp = temp->next;
    }
    return nullptr;
}


void initializeProgress() {
    addProgress("john123", 85, 12, "Physics");
    addProgress("mia456", 92, 15, "Math");
}


int main() {
    crow::SimpleApp app;

    initializeProgress();


    CROW_ROUTE(app, "/static/<string>")
    ([](std::string filename){
        return crow::response(crow::mustache::load("static/" + filename).render());
    });

   
    CROW_ROUTE(app, "/student_progress")
    ([](const crow::request& req){
        auto url = crow::query_string(req.url);
        std::string username = url.get("user") ? url.get("user") : "";

        ProgressNode* progress = findProgress(username);
        if (!progress) return crow::response(404, "Student progress not found");

        crow::mustache::context ctx;
        ctx["average_score"] = std::to_string(progress->averageScore) + "%";
        ctx["quizzes_completed"] = std::to_string(progress->quizzesCompleted);
        ctx["best_subject"] = progress->bestSubject;

        auto page = crow::mustache::load("student_progress.html");
        return page.render(ctx);
    });

    app.port(18080).multithreaded().run();
}
