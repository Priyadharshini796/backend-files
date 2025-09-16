
#include <crow.h>
#include <crow/mustache.h>
#include <string>


struct StudentNode {
    std::string username;
    std::string name;
    StudentNode* next;
    StudentNode* prev;

    StudentNode(const std::string& u, const std::string& n)
        : username(u), name(n), next(nullptr), prev(nullptr) {}
};

StudentNode* studentHead = nullptr;

void addStudent(const std::string& username, const std::string& name) {
    StudentNode* newNode = new StudentNode(username, name);
    if (!studentHead) {
        studentHead = newNode;
        return;
    }
    StudentNode* temp = studentHead;
    while (temp->next) temp = temp->next;
    temp->next = newNode;
    newNode->prev = temp;
}

StudentNode* findStudent(const std::string& username) {
    StudentNode* temp = studentHead;
    while (temp) {
        if (temp->username == username) return temp;
        temp = temp->next;
    }
    return nullptr;
}


void initializeStudents() {
    addStudent("john123", "John Doe");
    addStudent("mia456", "Mia Wallace");
}


int main() {
    crow::SimpleApp app;

    initializeStudents();

 
    CROW_ROUTE(app, "/static/<string>")
    ([](std::string filename){
        return crow::response(crow::mustache::load("static/" + filename).render());
    });

    
    CROW_ROUTE(app, "/student_dashboard")
    ([](const crow::request& req){
        auto url = crow::query_string(req.url);
        std::string username = url.get("user") ? url.get("user") : "";

        StudentNode* student = findStudent(username);
        if (!student) return crow::response(404, "Student not found");

        crow::mustache::context ctx;
        ctx["student_name"] = student->name;
        ctx["student_username"] = student->username;

        auto page = crow::mustache::load("student_dashboard.html");
        return page.render(ctx);
    });

    app.port(18080).multithreaded().run();
}
