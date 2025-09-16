
#include <crow.h>
#include <crow/mustache.h>
#include <string>
#include <list>
#include <random>
#include <sstream>


struct TeacherNode {
    std::string username;
    std::string name;
    TeacherNode* next;
    TeacherNode* prev;

    TeacherNode(const std::string& u, const std::string& n)
        : username(u), name(n), next(nullptr), prev(nullptr) {}
};

TeacherNode* head = nullptr;

void addTeacher(const std::string& username, const std::string& name) {
    TeacherNode* newNode = new TeacherNode(username, name);
    if (!head) {
        head = newNode;
        return;
    }
    TeacherNode* temp = head;
    while (temp->next) temp = temp->next;
    temp->next = newNode;
    newNode->prev = temp;
}

TeacherNode* findTeacher(const std::string& username) {
    TeacherNode* temp = head;
    while (temp) {
        if (temp->username == username) return temp;
        temp = temp->next;
    }
    return nullptr;
}



struct ClassroomNode {
    std::string className;
    std::string subject;
    std::string teacherUsername;
    std::string classCode;
    ClassroomNode* next;
    ClassroomNode* prev;

    ClassroomNode(const std::string& c, const std::string& s, const std::string& t, const std::string& code)
        : className(c), subject(s), teacherUsername(t), classCode(code), next(nullptr), prev(nullptr) {}
};

ClassroomNode* classHead = nullptr;

void addClassroom(const std::string& classname, const std::string& subject, const std::string& teacherUsername) {
    
    std::stringstream ss;
    ss << "CLS";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1000, 9999);
    ss << dis(gen);
    std::string code = ss.str();

    ClassroomNode* newNode = new ClassroomNode(classname, subject, teacherUsername, code);
    if (!classHead) {
        classHead = newNode;
        return;
    }
    ClassroomNode* temp = classHead;
    while (temp->next) temp = temp->next;
    temp->next = newNode;
    newNode->prev = temp;
}


void initializeTeachers() {
    addTeacher("alice", "Alice Johnson");
    addTeacher("bob", "Bob Smith");
    addTeacher("charlie", "Charlie Brown");
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

 
    CROW_ROUTE(app, "/create-class").methods(crow::HTTPMethod::Post)
    ([](const crow::request& req){
        auto body = crow::query_string(req.body);
        std::string classname = body.get("classname") ? body.get("classname") : "";
        std::string subject = body.get("subject") ? body.get("subject") : "";
        std::string teacherUsername = body.get("username") ? body.get("username") : "";

        if (classname.empty() || subject.empty() || teacherUsername.empty())
            return crow::response(400, "Missing fields");

        addClassroom(classname, subject, teacherUsername);

        std::string responseText = "Class '" + classname + "' created for teacher '" + teacherUsername + "'.";
        return crow::response(responseText);
    });

    app.port(18080).multithreaded().run();
}
