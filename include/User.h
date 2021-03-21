#ifndef USER_H_
#define USER_H_

#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include "Watchable.h"
#include "Session.h"


class Watchable;
class Session;

class User{
public:
    // Constructor
    User();
    User(std::string& name);

    // Destructor
    virtual ~User();

    // Copy Constructor
    User(User & other);


    // Move Constructor
    User(User && other);

    // Copy Assignment
    User& operator=(const User &other);

    // Move Assignment
    User& operator=( User &&other);

    virtual Watchable* getRecommendation(Session& s) = 0;
    std::string getName() const;
    std::vector<Watchable*> get_history() const;
    void deleteHistory();
    void addToHistory(Watchable* wa);
    virtual std::string getAlgorithmCode() =0;
    virtual void updateLastRecommendation();
    virtual void updateGenrePairs();
    void setHistory(std::vector<Watchable*> other);
    virtual User* clone()=0;


protected:
    std::vector<Watchable*> history;
    void clean();
    void copy( std::vector<Watchable*> history);
    void steal(User &other);


private:
    std::string name;

};



class LengthRecommenderUser : public User {
public:
    LengthRecommenderUser(std::string& name);
    LengthRecommenderUser(LengthRecommenderUser&other);
    virtual Watchable* getRecommendation(Session& s);
    virtual std::string getAlgorithmCode();
    void updateLastRecommendation();
    void updateGenrePairs();
    LengthRecommenderUser* clone();

};

class RerunRecommenderUser : public User {
public:
    RerunRecommenderUser(std::string& name);
    RerunRecommenderUser(RerunRecommenderUser&other);
    virtual Watchable* getRecommendation(Session& s);
    virtual std::string getAlgorithmCode();
    void updateLastRecommendation();
    void updateGenrePairs();
    RerunRecommenderUser* clone();

private:
    int lastRecommendation;
};

class GenreRecommenderUser : public User {
public:
    GenreRecommenderUser(std::string& name);
    GenreRecommenderUser(GenreRecommenderUser&other);
    virtual Watchable* getRecommendation(Session& s);
    virtual std::string getAlgorithmCode() ;
    void updateLastRecommendation();
    void updateGenrePairs();
    GenreRecommenderUser* clone();
private:
    std::vector<std::pair<std::string, int>> genrePairs;
};

#endif