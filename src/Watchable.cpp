
#include "../include/Watchable.h"
#include <iostream>
#include <string>
#include <sstream>
#include "../include/User.h"
#include "../include/Session.h"
using namespace std;


//class watchable
Watchable::Watchable(long id, int length, const std::vector<std::string>& tags) :
id(id), length(length), tags(tags){}

Watchable:: ~Watchable(){}

int Watchable::getLength() {
    return length;
}

long Watchable::getId() {
    return id;
}

vector<string> Watchable::getTags() {
    return tags;
}

string Watchable::getName(){}




//class movie
Movie::Movie(long id, const std::string& name, int length, const std::vector<std::string>& tags) : Watchable(id, length, tags), name(name) {}

string Movie:: toString() const{
    return name;
    stringstream str1;
    str1<< name ;
    string s= str1.str();
    return s;
}

Watchable* Movie::  getNextWatchable(Session& s) const{
//    User* user = s.getActiveuser();
//    Watchable* wa = s.getActiveuser()->getRecommendation(s);
    return s.getActiveuser()->getRecommendation(s);
}

int Movie::getLength() {}

string Movie::getName() {
    return name;
}

Movie *Movie::clone() {
    return new Movie(*this);
}

Movie::Movie(Movie &other) :Watchable(other),name(other.name){

}


//class episode
Episode::Episode(long id, const std::string& seriesName,int length, int season, int episode ,const std::vector<std::string>& tags) : Watchable(id, length, tags),
season(season), episode(episode), seriesName(seriesName), nextEpisodeId(id+1){}

string Episode:: toString() const{

    stringstream str1;
    str1<< seriesName << " S" << season << "E" << episode ;
    string s= str1.str();
    return s;
}

Watchable* Episode:: getNextWatchable(Session& s) const{
//    User* user = s.getActiveuser();
//    vector<Watchable*> content = s.getContent();
//    Watchable* thisEpisode =s.getContent()[nextEpisodeId-2];
//    Watchable* nextEpisode = s.getContent()[nextEpisodeId-1];
//    string currentSeriesName = s.getContent()[nextEpisodeId-2]->getName();
    if(s.getContent()[nextEpisodeId-1] == nullptr){
//        Watchable* wa2 = s.getActiveuser()->getRecommendation(s);
        return s.getActiveuser()->getRecommendation(s);
    }
    if(nextEpisodeId > s.getContent().size()) {
//        Watchable* wa3 = s.getActiveuser()->getRecommendation(s);
        return s.getActiveuser()->getRecommendation(s);
    }
//    string seriesName = s.getContent()[nextEpisodeId-1]->getName();
//    string code = s.getActiveuser()->getAlgorithmCode();
    bool exist = false;
    if(s.getActiveuser()->getAlgorithmCode() != "rer"){
//        vector<Watchable*> history = s.getActiveuser()->get_history();
        for (int i = 0; i < s.getActiveuser()->get_history().size() && !exist; ++i) {
            if(s.getActiveuser()->get_history()[i] == s.getContent()[nextEpisodeId-1]){
                exist = true;
            }
        }
    }
    if(!exist){
        if(s.getContent()[nextEpisodeId-2]->getName() == s.getContent()[nextEpisodeId-1]->getName()){
            return s.getContent()[nextEpisodeId-1];
        }
    }
//    Watchable* wa = s.getActiveuser()->getRecommendation(s);
    return s.getActiveuser()->getRecommendation(s);
}


int Episode::getLength() {}

string Episode::getName() {
    return seriesName;
}

Episode::Episode(Episode &other) :Watchable(other),seriesName(other.seriesName),season(other.season),episode(other.episode),nextEpisodeId(other.nextEpisodeId){

}

Episode *Episode::clone() {
    return new Episode(*this);
}

