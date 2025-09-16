
#include <crow.h>
#include <crow/mustache.h>
#include <string>


struct TeacherNode {
    std::string username;
    std::string name;
    TeacherNode* next;
    TeacherNode* prev;

    TeacherNode(const std::string& u, const std::string& n)
        : username(u), name(n), next(nullptr), prev(nullptr) {}
};

TeacherNode* teacherHead = nullptr;

void addTeacher(const std::string& username, const std::string& name) {
    TeacherNode* newNode = new TeacherNode(username, name);
    if (!teacherHead) {
        teacherHead = newNode;
        return;
    }
    TeacherNode* temp = teacherHead;
    while (temp->next) temp = temp->next;
    temp->next = newNode;
    newNode->prev = temp;
}

TeacherNode* findTeacher(const std::string& username) {
    TeacherNode* temp = teacherHead;
    while (temp) {
        if (temp->username == username) return temp;
        temp = temp->next;
    }
    return nullptr;
}


void initializeTeachers() {
    addTeacher("smith001", "John Smith");
    addTeacher("doe002", "Jane Doe");
}


int main() {
    crow::SimpleApp app;

    initializeTeachers();


    CROW_ROUTE(app, "/static/<string>")
    ([](std::string filename){
        return crow::response(crow::mustache::load("static/" + filename).render());
    });

   
    CROW_ROUTE(app, "/teacher_dashboard")
    ([](const crow::request& req){
        auto url = crow::query_string(req.url);
        std::string username = url.get("user") ? url.get("user") : "";

        TeacherNode* teacher = findTeacher(username);
        if (!teacher) return crow::response(404, "Teacher not found");

        crow::mustache::context ctx;
        ctx["teacher_name"] = teacher->name;
        ctx["teacher_username"] = teacher->username;

        auto page = crow::mustache::load("teacher_dashboard.html");
        return page.render(ctx);
    });

    app.port(18080).multithreaded().run();
}
