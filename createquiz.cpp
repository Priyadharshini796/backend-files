
#include <crow.h>
#include <crow/mustache.h>
#include <string>
#include <sstream>
#include <random>


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


struct ClassroomNode {
    std::string id;  
    std::string name;
    std::string subject;
    std::string teacherUsername;
    ClassroomNode* next;
    ClassroomNode* prev;

    ClassroomNode(const std::string& i, const std::string& n, const std::string& s, const std::string& t)
        : id(i), name(n), subject(s), teacherUsername(t), next(nullptr), prev(nullptr) {}
};
ClassroomNode* classHead = nullptr;

std::string generateClassId() {
    std::stringstream ss;
    ss << "CLS";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1000, 9999);
    ss << dis(gen);
    return ss.str();
}

void addClassroom(const std::string& name, const std::string& subject, const std::string& teacherUsername) {
    std::string id = generateClassId();
    ClassroomNode* newNode = new ClassroomNode(id, name, subject, teacherUsername);
    if (!classHead) {
        classHead = newNode;
        return;
    }
    ClassroomNode* temp = classHead;
    while (temp->next) temp = temp->next;
    temp->next = newNode;
    newNode->prev = temp;
}


std::vector<ClassroomNode*> getTeacherClassrooms(const std::string& teacherUsername) {
    std::vector<ClassroomNode*> res;
    ClassroomNode* temp = classHead;
    while (temp) {
        if (temp->teacherUsername == teacherUsername) res.push_back(temp);
        temp = temp->next;
    }
    return res;
}


void initializeTeachersAndClasses() {
    addTeacher("alice", "Alice Johnson");
    addTeacher("bob", "Bob Smith");

    addClassroom("Grade 10 Physics", "Physics", "alice");
    addClassroom("Grade 11 Chemistry", "Chemistry", "alice");
    addClassroom("Grade 9 Math", "Mathematics", "bob");
}


int main() {
    crow::SimpleApp app;
    initializeTeachersAndClasses();


    CROW_ROUTE(app, "/static/<string>")
    ([](std::string filename){
        return crow::response(crow::mustache::load("static/" + filename).render());
    });


    CROW_ROUTE(app, "/create-quiz-dashboard")
    ([](const crow::request& req){
        auto url = crow::query_string(req.url);
        std::string username = url.get("user") ? url.get("user") : "";

        TeacherNode* teacher = findTeacher(username);
        if (!teacher) return crow::response(404, "Teacher not found");

  
        auto classrooms = getTeacherClassrooms(username);

        crow::mustache::context ctx;
        ctx["teacher_name"] = teacher->name;
        ctx["teacher_username"] = teacher->username;

        std::vector<crow::mustache::context> clsArr;
        for (auto c : classrooms) {
            crow::mustache::context cctx;
            cctx["id"] = c->id;
            cctx["name"] = c->name;
            cctx["subject"] = c->subject;
            clsArr.push_back(cctx);
        }
        ctx["classrooms"] = clsArr;

        auto page = crow::mustache::load("create_quiz.html");
        return page.render(ctx);
    });

    CROW_ROUTE(app, "/create-quiz").methods(crow::HTTPMethod::Post)
    ([](const crow::request& req){
        auto body = crow::query_string(req.body);
        std::string quizTitle = body.get("quiz_title") ? body.get("quiz_title") : "";
        std::string classroomId = body.get("classroom_id") ? body.get("classroom_id") : "";

        if (quizTitle.empty() || classroomId.empty())
            return crow::response(400, "Missing fields");

        std::string responseText = "Quiz '" + quizTitle + "' created for classroom '" + classroomId + "'.";
        return crow::response(responseText);
    });

    app.port(18080).multithreaded().run();
}
