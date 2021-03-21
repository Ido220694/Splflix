
#include "../include/Session.h"
#include <iostream>
#include "../include/json.hpp"
#include <fstream>
#include "../include/Watchable.h"
#include "../include/Action.h"
#include "../include/User.h"
#include <string>
#include <sstream>


using namespace std;

Session:: Session(const std::string &configFilePath)  {
    createDefaultUserBool = true;
// read a JSON file
    using json = nlohmann::json;
    ifstream inFromStream(configFilePath);
    json j;
    inFromStream >>j;
    long count=0;
    for(; count<j["movies"].size(); ++count){
        Watchable *mo = new Movie(count+1, j["movies"][count]["name"], j["movies"][count]["length"], j["movies"][count]["tags"]);
        content.push_back(mo);
    }
    for(int i=0; i<j["tv_series"].size(); i++){
        for (int s=0; s<j["tv_series"][i]["seasons"].size(); s++){
            for( int e=0; e<j["tv_series"][i]["seasons"][s]; e++){
                Watchable *ep = new Episode(count+1, j["tv_series"][i]["name"], j["tv_series"][i]["episode_length"], s+1, e+1, j["tv_series"][i]["tags"]);
                content.push_back(ep);
                count++;
            }
        }
    }
}


// Destructor
Session:: ~Session() { clean(); }


void Session::clean() {
    for(int i=0; i<content.size(); ++i){
        delete(this->content[i]);
        this->content[i] = nullptr;
    }
    for(int i=0; i<actionsLog.size(); ++i){
        delete(this->actionsLog[i]);
        this->actionsLog[i] = nullptr;
    }
    for (auto &x : userMap) {
        User* u= x.second;
        delete(u);
        u=nullptr;
    }

    this->content.clear();
    this->actionsLog.clear();
    this->userMap.clear();
}


//Copy Constructor
Session:: Session(Session &other){
    copy(other);
    createDefaultUserBool = false;
}

void Session::copy(const Session &other) {
    //copy content
    for (unsigned int i = 0; i < other.getContent().size(); ++i) {
        this->content.push_back(other.getContent()[i]->clone());
    }

    //copy actions log
    for (unsigned int i = 0; i < other.getActionLog().size(); ++i) {
        //this->actionsLog.push_back(other.getActionLog()[i]->clone());
        BaseAction* clonedAction = other.getActionLog()[i]->clone();
        this->actionsLog.push_back(clonedAction);
        clonedAction= nullptr;
    }

    //copy users
    for( auto x : other.getUserMap() ){
        User* clonedUser = x.second->clone();

        pair<string, User*> insert(x.first, clonedUser);
        this->AddToMap(insert);
        clonedUser->deleteHistory();

        for(int i = 0; i < x.second->get_history().size(); ++i){
            bool found = false;
            for (int j = 0; j < content.size() && !found; ++j) {
                if(x.second->get_history()[i]->getId() == content[j]->getId()){
                    clonedUser->addToHistory(content[j]);
                    found = true;
                }
            }

        }
    }
    string newUserName = other.getActiveuser()->getName();
    this->activeUser = this->userMap[newUserName];
}

// Move Constructor

Session::Session(Session&& other) {
    steal(other);
}

void Session::steal(Session &other) {
    this->activeUser = other.getActiveuser();
    for (int j = 0; j < other.getContent().size(); ++j) {
        content.push_back(other.getContent()[j]);
        other.getContent()[j] = nullptr;
    }
    for (int i = 0; i < other.getActionLog().size(); ++i) {
        actionsLog.push_back(other.getActionLog()[i]);
        other.getContent()[i] = nullptr;
    }
    for( auto x : other.getUserMap() ){
        pair<string, User*> insert(x.first, x.second);
        this->AddToMap(insert);
        x.second = nullptr;
    }

    other.getContent().clear();
    other.getActionLog().clear();
    other.getUserMap().clear();
}

// Copy Assignment
Session& Session::operator=(const Session &other){
    if(this == &other) {
        return *this;
    }
    clean();
    copy(other);
    return *this;
}

// Move Assignment
Session& Session::operator=(Session &&other) {
    if(this!= &other){
        clean();
        steal(other);
    }
    return *this;
}



User* Session::getActiveuser() const {
    return activeUser;}

void Session:: start(){
    cout<<"SPLFLIX is now on!" <<endl;

    bool loop = true;
    string name="default";
    string rec="len";
    if(createDefaultUserBool) {
        CreateUser(name, rec).act(*this);
    }

    activeUser = getUser(name);

    while(loop){
        string inputS;
        getline(cin, inputS);
        istringstream input2(inputS);
        vector<string> words((istream_iterator<string>(input2)), istream_iterator<string>());

        if(words[0]=="createuser"){
            CreateUser* user = new CreateUser(words[1], words[2]);
            user->act(*this);
            pushToActionlog(user);
        }

        if(words[0] =="changeuser"){
            ChangeActiveUser* change = new ChangeActiveUser(words[1]);
            change->act(*this);
            pushToActionlog(change);
        }

        if(words[0] == "deleteuser"){
            DeleteUser* deleteUser = new DeleteUser(words[1]);
            deleteUser->act(*this);
            pushToActionlog(deleteUser);
        }

        if(words[0] == "dupuser"){
            DuplicateUser* toDuplicate = new DuplicateUser(words[1], words[2]);
            toDuplicate->act(*this);
            pushToActionlog(toDuplicate);
        }

        if(words[0] == "content"){
            PrintContentList* print = new PrintContentList();
            print->act(*this);
            pushToActionlog(print);
        }

        if(words[0] == "watchhist"){
            PrintWatchHistory* printWatch = new PrintWatchHistory();
            printWatch->act(*this);
            pushToActionlog(printWatch);
        }

        if(words[0] == "watch"){
            long id = stol (words[1], nullptr,  10);
            Watch* toWatch = new Watch(id);
            toWatch->act(*this);
            pushToActionlog(toWatch);

            bool ans = true;
            if(toWatch->getStatus() == COMPLETED){
                while (ans){
                    string answer;
                    getline(cin, answer);
                    if(answer == "y"){
                        ans = true;
                        string rer = "rer" ;
                        if(rer == getActiveuser()->getAlgorithmCode()){
                            getActiveuser()->updateLastRecommendation();
                        }
                        string gen = "gen" ;
                        if(gen == getActiveuser()->getAlgorithmCode()){
                            getActiveuser()->updateGenrePairs();
                        }

                        long id2 = getContent()[id - 1]->getNextWatchable(*this)->getId();
                        id =id2;

                        Watch* toWatch2 = new Watch(id2);
                        toWatch2->act(*this);
                        pushToActionlog(toWatch2);
                    }
                    else if(answer == "n"){
                        ans = false;
                    }
                    else {
                        cout << "invalid input" << endl;
                    }
                }
            }

        }

        if(words[0] =="log"){
            PrintActionsLog* printLog = new PrintActionsLog();
            printLog->act(*this);
            pushToActionlog(printLog);
        }

        if(words[0] == "exit"){
            Exit* x = new Exit();
            x->act(*this);
            pushToActionlog(x);
            loop = false;
        }
    }
}


unordered_map<std::string,User*> Session:: getUserMap() const{
    return userMap;
}

vector<Watchable*> Session::getContent() const{
    return content;
}


vector<BaseAction *> Session::getActionLog() const{
    return actionsLog;
}

void Session::setActiveUser(User* other) {
    activeUser = other;
}

void Session::AddToMap(pair<std::string,User*> other) {
    userMap.insert(other);
}

void Session::DeleteFromMap(string other) {
    User * toDelete = getUser(other);
    delete(toDelete);
    toDelete = nullptr;
    userMap.erase(other);
}

void Session::pushToActionlog(BaseAction *other) {
    actionsLog.push_back(other);
}

User* Session::getUser(string name)  {
    if(userMap.find(name) == userMap.end()){
        return nullptr;
    }
    return userMap[name];
}






