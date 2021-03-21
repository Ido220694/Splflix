#ifndef SESSION_H_
#define SESSION_H_

#include <vector>
#include <unordered_map>
#include <string>
#include "Action.h"
#include "User.h"

class User;
class Watchable;

class Session{
public:
    // Constructor
    Session(const std::string &configFilePath);

    // Destructor
    ~Session();

    // Copy Constructor
    Session(Session & other);

    // Move Constructor
    Session(Session && other);

    // Copy Assignment
    Session& operator=(const Session &other);

    // Move Assignment
    Session& operator=( Session &&other);


    void start();

    User* getActiveuser() const;

    std::unordered_map<std::string,User*> getUserMap() const;
    std::vector<Watchable*> getContent() const;
    std::vector<BaseAction*> getActionLog() const;

    void setActiveUser(User* other);
    void AddToMap(std::pair<std::string,User*> other);
    void DeleteFromMap(std::string other);
    void pushToActionlog(BaseAction* other);
    User* getUser(std::string name);

    void clean();
    void copy(const Session &other);
    void steal(Session &other);

private:
    std::vector<Watchable*> content;
    std::vector<BaseAction*> actionsLog;
    std::unordered_map<std::string,User*> userMap;
    User* activeUser;
    bool createDefaultUserBool;
};
#endif