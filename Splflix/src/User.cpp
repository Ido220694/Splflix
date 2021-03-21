#include "../include/User.h"
#include <iostream>
#include<algorithm>
#include<string.h>
#include "../include/Session.h"
#include "../include/Watchable.h"

using namespace std;


//class user
User:: User(){}

// Constructor
User:: User(std::string& name):name(name) {

}

// Destructor
User::~User() { clean(); }


void User::clean() {
    for(size_t i=0; i<history.size(); ++i){
        this->history[i] = nullptr;
    }
    this->name.clear();
    this->history.clear();
}


// Copy Constructor
User:: User(User &other) : name(other.getName()){
    copy(other.get_history());
}

void User::copy(std::vector<Watchable*> history) {
    for (int i = 0; i < history.size(); ++i) {
        Watchable* wa = history.at(i);
        this->history.push_back(wa);
    }
}

// Move Constructor
User::User(User&& other): name() {
    steal(other);
}

void User::steal(User &other) {
    this->name = other.getName();
    for (int j = 0; j < other.get_history().size(); ++j) {
        history.push_back(other.get_history()[j]);
        other.get_history()[j] = nullptr;
    }

    other.history.clear();
    other.name.clear();
}

// Copy Assignment
User& User::operator=(const User &aUser){
    if(this == &aUser) {
        return *this;
    }

    clean();
    copy(aUser.get_history());
    return *this;
}

// Move Assignment
User& User::operator=(User &&other) {
    if(this!= &other){
        clean();
        steal(other);
    }
    return *this;
}



string User:: getName() const{
    return this->name;
}

vector<Watchable*> User:: get_history() const{
    return history;
}

void User::addToHistory(Watchable *wa) {
    history.push_back(wa);
}

void User::updateLastRecommendation() {}

void User::updateGenrePairs() {}

void User::setHistory(vector<Watchable *> other) {
    for (int i = 0; i < other.size() ; ++i) {
        this->addToHistory(other[i]);
    }
}

void User::deleteHistory() {
    history.clear();
}



//class LengthRecommenderUser
LengthRecommenderUser:: LengthRecommenderUser(string& name): User(name) {}

string LengthRecommenderUser::getAlgorithmCode() {
    return "len";
}

Watchable* LengthRecommenderUser::  getRecommendation(Session& s){
     int avg;
     int sum=0;
     if(get_history().size()==0){
         return nullptr;
     }


    if(get_history().size() == s.getContent().size()){
         return nullptr;
     }

    //calculate average length of history content
     for(int i=0; i<get_history().size(); ++i){
         int tmp = get_history()[i]->getLength();
         sum = sum + tmp;
     }
     avg = sum / get_history().size();
    int minDiff = 999999;
    Watchable* min = nullptr;
    long lastId = get_history()[get_history().size() - 1 ]->getId();
    for(int i = 0; i < s.getContent().size() ; ++i) {
        bool hist = false;
        for(int j=0; j<get_history().size() && !hist; ++j) {
            if (s.getContent()[i] == get_history()[j]) {
                hist = true;
            }
        }
        if(hist == false){
            if (lastId != s.getContent()[i]->getId()) {
                int diff;
                diff = s.getContent()[i]->getLength() - avg;
                if (diff < 0) {
                    diff = diff * (-1);
                }
                if (diff < minDiff) {
                    minDiff = diff;
                    min = s.getContent()[i];
                }
            }
        }
    }
    return min;

}

void LengthRecommenderUser::updateLastRecommendation() {}

void LengthRecommenderUser::updateGenrePairs() {}

LengthRecommenderUser::LengthRecommenderUser(LengthRecommenderUser &other) : User(other) {

}

LengthRecommenderUser *LengthRecommenderUser::clone() {
    return new LengthRecommenderUser(*this);
}





//class RerunRecommenderUser

RerunRecommenderUser:: RerunRecommenderUser(string& name): User(name) , lastRecommendation(0){}

Watchable* RerunRecommenderUser:: getRecommendation(Session& s){
    return get_history()[lastRecommendation % get_history().size()];
}

string RerunRecommenderUser::getAlgorithmCode() {
    return "rer";
}

void RerunRecommenderUser::updateLastRecommendation() {
    lastRecommendation = lastRecommendation + 1;
}

void RerunRecommenderUser::updateGenrePairs() {}

RerunRecommenderUser::RerunRecommenderUser(RerunRecommenderUser &other) :User(other), lastRecommendation(0) {

}

RerunRecommenderUser *RerunRecommenderUser::clone() {
    return new RerunRecommenderUser(*this);

}





//class GenreRecommenderUser

GenreRecommenderUser:: GenreRecommenderUser(string& name):User(name){}

Watchable* GenreRecommenderUser::  getRecommendation(Session& s){

    if(history.size()==0){
        return nullptr;
    }
    vector<Watchable*> content = s.getContent();
    if(get_history().size() == content.size()){
        return nullptr;
    }

    // update the histogram accroding to the last watch
    int i=history.size() - 1;
    vector<string> genres = get_history()[i]->getTags();
    for(int j = 0; j < genres.size(); ++j){
        string tag = genres[j];
        bool tagExists = false;
        for(int k = 0; k < genrePairs.size(); ++k) {
            if (tag == genrePairs[k].first) {
                genrePairs[k].second++;
                tagExists = true;
            }
        }
        if(!tagExists){
            pair<string, int> tag1 (tag, 1);
            genrePairs.push_back(tag1);
        }
        // sort the vector
        sort(genrePairs.begin(), genrePairs.end(), [](const pair<string,int> &left, const pair<string,int> &right){
            if(left.second != right.second){
                return left.second >right.second;
            }
            return left.first < right.first;
        });
    }
    string maxTag;
    for( int l=0; l<genrePairs.size() ; ++l){
        maxTag = genrePairs[l].first;
        for(int i=0; i<content.size() ; ++i){
            vector<string> genres = content[i]->getTags();
            bool repeat = false;
            for(int j=0; j< get_history().size() && !repeat; ++j) {
                long tmp = get_history()[j]->getId();
                long tmp2 = content[i]->getId();
                if (tmp == tmp2) {
                    repeat = true;
                }
            }
            if(!repeat){
                for(int t=0; t< genres.size() ; ++t){
                if(genres[t] == maxTag) {
                    return content[i];
                    }
                }
            }
        }
    }
}

string GenreRecommenderUser::getAlgorithmCode() {

    string str= "gen";
    return str;
}

void GenreRecommenderUser::updateLastRecommendation() {}

void GenreRecommenderUser::updateGenrePairs() {
    int i=get_history().size() - 1;
    vector<string> genres = get_history()[i]->getTags();
    for(int j = 0; j < genres.size(); ++j){
        string tag = genres[j];
        bool tagExists = false;
        for(int k = 0; k < genrePairs.size(); ++k) {
            if (tag == genrePairs[k].first) {
                genrePairs[k].second++;
                tagExists = true;
            }
        }
        if(!tagExists){
            pair<string, int> tag1 (tag, 1);
            genrePairs.push_back(tag1);
        }
        // sort the vector
        sort(genrePairs.begin(), genrePairs.end(), [](const pair<string,int> &left, const pair<string,int> &right){
            if(left.second != right.second){
                return left.second >right.second;
            }
            return left.first < right.first;
        });
    }
}

GenreRecommenderUser::GenreRecommenderUser(GenreRecommenderUser &other) :User(other) {
}

GenreRecommenderUser *GenreRecommenderUser::clone() {
    return new GenreRecommenderUser(*this);

}




