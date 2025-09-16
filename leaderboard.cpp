
#include <crow.h>
#include <crow/mustache.h>
#include <string>
#include <vector>
#include <sstream>
#include <random>
struct LeaderboardEntry {
    std::string name;
    int score; 
    std::string timeTaken;
    LeaderboardEntry* next;
    LeaderboardEntry* prev;

    LeaderboardEntry(const std::string& n, int s, const std::string& t)
        : name(n), score(s), timeTaken(t), next(nullptr), prev(nullptr) {}
};

LeaderboardEntry* leaderboardHead = nullptr;

void addLeaderboardEntry(const std::string& name, int score, const std::string& time) {
    LeaderboardEntry* newNode = new LeaderboardEntry(name, score, time);
    if (!leaderboardHead) {
        leaderboardHead = newNode;
        return;
    }
    LeaderboardEntry* temp = leaderboardHead;
    while (temp->next) temp = temp->next;
    temp->next = newNode;
    newNode->prev = temp;
}

void initializeLeaderboard() {
    addLeaderboardEntry("Mia Wallace", 100, "2:15");
    addLeaderboardEntry("Alex Johnson", 90, "3:05");
    addLeaderboardEntry("Ben Carter", 90, "3:45");
    addLeaderboardEntry("Chloe Davis", 80, "4:01");
}


int main() {
    crow::SimpleApp app;

    initializeLeaderboard();

    CROW_ROUTE(app, "/static/<string>")
    ([](std::string filename){
        return crow::response(crow::mustache::load("static/" + filename).render());
    });

    CROW_ROUTE(app, "/leaderboard")
    ([](){
        crow::mustache::context ctx;

       
        std::vector<crow::mustache::context> entries;
        LeaderboardEntry* temp = leaderboardHead;
        int rank = 1;
        while (temp) {
            crow::mustache::context entryCtx;
            entryCtx["rank"] = rank;
            entryCtx["name"] = temp->name;
            entryCtx["score"] = std::to_string(temp->score) + "%";
            entryCtx["time"] = temp->timeTaken;

          
            if (temp->name == "Alex Johnson") entryCtx["current_user"] = true;
            else entryCtx["current_user"] = false;

            entries.push_back(entryCtx);
            temp = temp->next;
            rank++;
        }
        ctx["leaderboard"] = entries;

        auto page = crow::mustache::load("leaderboard.html");
        return page.render(ctx);
    });

    app.port(18080).multithreaded().run();
}
