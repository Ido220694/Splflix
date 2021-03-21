
#include <string>
#include "../include/Action.h"

#include "../include/Session.h"
#include "../include/User.h"
#include <sstream>
#include <unordered_map>
#include "../include/Watchable.h"
#include <iostream>
#include <fstream>


using namespace std;

//class BaseAction
BaseAction::BaseAction() {
    status=PENDING;
}

ActionStatus BaseAction::getStatus() const{
    return status;
}

void BaseAction::act(Session& sess){

}

string BaseAction::toString() const {
}

void BaseAction::complete(){
    status = COMPLETED;
}

void BaseAction:: error(const std::string& errorMsg){
    status = ERROR;
}

string BaseAction:: getErrorMsg() const{
    return errorMsg;
}

void BaseAction::setErrorMsg(string s) {
    errorMsg = s;
}

BaseAction::BaseAction(BaseAction &other) : errorMsg(other.errorMsg), status(other.status) {

}

BaseAction::~BaseAction() {

}


//ACTION CREATE USER
CreateUser::CreateUser(string name, string algoCode): userName(name), code(algoCode) {}

void CreateUser:: act(Session& sess){
    User* check = sess.getUser(userName);
    if (code != "len" && code != "rer" && code != "gen"){
        setErrorMsg("Invalid recommendation algorithm code");
        error(getErrorMsg());
        cout<<"Error - " + getErrorMsg() + "\n";

    }
    else {
        if(sess.getUser(userName) == nullptr){
            User* newUser;

            if(code == "rer"){
                newUser = new RerunRecommenderUser(userName);
            }
            else if(code == "len"){
                newUser = new LengthRecommenderUser(userName);
            }
            else{
                newUser = new GenreRecommenderUser(userName);
            }

            pair<string, User*> newUserInsert(userName, newUser);

            sess.AddToMap(newUserInsert);
            complete();
        }

        else{
            setErrorMsg("username is already in use");
            error(getErrorMsg());
            cout<<"Error - " + getErrorMsg() + "\n";
        }
    }

}
string CreateUser:: toString() const{
    stringstream str1;
    str1<< "CreateUser " ;
    if(getStatus() == COMPLETED){
        str1 <<"COMPLETED";
    }
    if(getStatus() == PENDING){
        str1<<"PENDING";
    }
    if(getStatus() == ERROR){
        str1<< "ERROR: ";
        str1<< getErrorMsg();
    }
    string s = str1.str();
    return s;
}


CreateUser *CreateUser::clone() {
    return new CreateUser(*this);
}

CreateUser::CreateUser(CreateUser &other) : userName(other.userName), code(other.code){
    if(other.getStatus() == COMPLETED){
        complete();
    }
    if(other.getStatus() == ERROR){
        setErrorMsg(other.getErrorMsg());
        error(getErrorMsg());
    }
}

//ACTION CHANGE ACTIVE USER
ChangeActiveUser::ChangeActiveUser(string name):userName(name) {}

void ChangeActiveUser:: act(Session& sess){
    User* newActive = sess.getUser(userName);

    if(newActive == nullptr){
        setErrorMsg("username does not exist");
        error(getErrorMsg());
        cout<<"Error - " + getErrorMsg()+ "\n";
    }
    else{
        sess.setActiveUser(newActive);
        complete();
    }
}

 string ChangeActiveUser:: toString() const{
     stringstream str1;
     str1<< "ChangeUser " ;
     if(getStatus() == COMPLETED){
         str1 <<"COMPLETED";
     }
     if(getStatus() == PENDING){
         str1<<"PENDING";
     }
     if(getStatus() == ERROR){
         str1<< "ERROR: ";
         str1<< getErrorMsg();
     }
     string s = str1.str();
     return s;
}


ChangeActiveUser *ChangeActiveUser::clone() {
    return new ChangeActiveUser(*this);
}

ChangeActiveUser::ChangeActiveUser(ChangeActiveUser &other) :userName(other.userName) {
    if(other.getStatus() == COMPLETED){
        complete();
    }
    if(other.getStatus() == ERROR){
        setErrorMsg(other.getErrorMsg());
        error(getErrorMsg());
    }
}

//ACTION DELETE USER
DeleteUser::DeleteUser(string name): userName(name){}
void DeleteUser:: act(Session & sess){
    if(sess.getUser(userName) == nullptr){
        setErrorMsg("This User does not exist");
        error(getErrorMsg());
        cout<<"Error - " + getErrorMsg() + "\n";
    }
    else{
        sess.DeleteFromMap(userName);
        complete();
    }
}
string DeleteUser:: toString() const{

    stringstream str1;
    str1<< "DeleteUser " ;
    if(getStatus() == COMPLETED){
        str1 <<"COMPLETED";
    }
    if(getStatus() == PENDING){
        str1<<"PENDING";
    }
    if(getStatus() == ERROR){
        str1<< "ERROR: ";
        str1<< getErrorMsg();
    }
    string s = str1.str();
    return s;
}


DeleteUser *DeleteUser::clone() {
    return new DeleteUser(*this);

}

DeleteUser::DeleteUser(DeleteUser &other) : userName(other.userName){
    if(other.getStatus() == COMPLETED){
        complete();
    }
    if(other.getStatus() == ERROR){
        setErrorMsg(other.getErrorMsg());
        error(getErrorMsg());
    }
}

//ACTION DUPLICATE USER
DuplicateUser::DuplicateUser(string original, string newName) : userName(newName), originalUser(original){}
void DuplicateUser::act(Session & sess){


    if(sess.getUser(originalUser) == nullptr){
        setErrorMsg("username does not exist");
        error(getErrorMsg());
        cout<<"Error - " + getErrorMsg() + "\n";
    }

    else if(sess.getUser(userName) != nullptr){
        setErrorMsg("username is already in use");
        error(getErrorMsg());
        cout<<"Error - " + getErrorMsg() +"\n";
    }

    else {
        string originalUserName = originalUser;
        User* newUser;

        string algCode = sess.getUser(originalUser)->getAlgorithmCode();

        if(algCode == "rer"){
            newUser = new RerunRecommenderUser(userName);
        }

        else if(algCode == "len"){
            newUser = new LengthRecommenderUser(userName);
        }
        else{
            newUser = new GenreRecommenderUser(userName);
        }

        pair<string, User*> newUserInsert(userName, newUser);

        sess.AddToMap(newUserInsert);
        newUser->setHistory(sess.getUser(originalUser)->get_history());
        complete();
    }
}

string DuplicateUser:: toString() const{

    stringstream str1;
    str1<< "DuplicateUser " ;
    if(getStatus() == COMPLETED){
        str1 <<"COMPLETED";
    }
    if(getStatus() == PENDING){
        str1<<"PENDING";
    }
    if(getStatus() == ERROR){
        str1<< "ERROR: ";
        str1<< getErrorMsg();
    }
    string s = str1.str();
    return s;
}


DuplicateUser *DuplicateUser::clone() {
    return new DuplicateUser(*this);
}

DuplicateUser::DuplicateUser(DuplicateUser &other) : userName(other.userName), originalUser(other.originalUser) {
    if(other.getStatus() == COMPLETED){
        complete();
    }
    if(other.getStatus() == ERROR){
        setErrorMsg(other.getErrorMsg());
        error(getErrorMsg());
    }
}

//ACTION PRINT CONTENT LIST
PrintContentList::PrintContentList(){}
void PrintContentList:: act (Session& sess) {
    for (int i = 0; i < sess.getContent().size(); ++i) {
        stringstream str1;
        Watchable *wa = sess.getContent()[i];
        str1 << sess.getContent()[i]->getId() << ". " ;
        str1 << wa->toString() << " " << wa->getLength() << " minutes [";
        vector<string> tags = wa->getTags();
        for (int j = 0; j < tags.size() - 1; ++j) {
            str1 << tags[j] << ", ";
        }
        str1 << tags[tags.size() - 1];
        str1 << "]" << "\n";
        string output = str1.str();
        cout << output;
    }
    complete();
}

string PrintContentList:: toString() const{

    stringstream str1;
    str1<< "PrintContentList " ;
    if(getStatus() == COMPLETED){
        str1 <<"COMPLETED";
    }
    if(getStatus() == PENDING){
        str1<<"PENDING";
    }
    if(getStatus() == ERROR){
        str1<< "ERROR: ";
    }
    string s = str1.str();
    return s;
}


PrintContentList *PrintContentList::clone() {
    return new PrintContentList(*this);

}

PrintContentList::PrintContentList(PrintContentList &other) {
    if(other.getStatus() == COMPLETED){
        complete();
    }
}


//ACTION PRINT WATCH HISTORY
PrintWatchHistory::PrintWatchHistory(){}
void PrintWatchHistory:: act (Session& sess){
    User* user = sess.getActiveuser();
    vector<Watchable *> history = user->get_history();
    cout<<"Watch History for ";
    cout<< sess.getActiveuser()->getName();
    cout<< "\n";
    for (int i = 0; i < history.size(); ++i) {
        stringstream str1;
        Watchable *wa = history[i];
        str1 << i+1 << ". "<< wa->toString() << "\n";
        string output = str1.str();
        cout << output;
    }

    complete();
}

 string PrintWatchHistory:: toString() const{
     stringstream str1;
     str1<< "PrintHistoryList " ;
     if(getStatus() == COMPLETED){
         str1 <<"COMPLETED";
     }
     if(getStatus() == PENDING){
         str1<<"PENDING";
     }
     if(getStatus() == ERROR){
         str1<< "ERROR: ";
     }
     string s = str1.str();
     return s;
}


PrintWatchHistory *PrintWatchHistory::clone() {
    return new PrintWatchHistory(*this);

}

PrintWatchHistory::PrintWatchHistory(PrintContentList &other) {
    if(other.getStatus() == COMPLETED){
        complete();
    }
}

//ACTION WATCH
Watch::Watch(long id) : id(id){}

void Watch:: act(Session& sess){
    if(id < 0 || id > sess.getContent().size()){
        setErrorMsg("This id does not exist");
        error(getErrorMsg());
        cout<<"Error - " + getErrorMsg() + "\n"<<endl;
    }

    else{

        stringstream str1;

        unsigned long id_l=(unsigned long) id;

        string s= sess.getContent().at(id_l-1)->toString();

        str1<< "Watching " << s ;
        complete();
        string stri = str1.str();
        cout<<stri<<endl;

        sess.getActiveuser()->addToHistory(sess.getContent()[id - 1]);
        contentWatch = s;
        stringstream str2;
        str2<< "We recommend watching "<< sess.getContent()[id - 1]->getNextWatchable(sess)->toString() << " continue watching[y/n]" ;
        string stri2 = str2.str();
        cout << stri2<<endl;
    }
}
 string Watch:: toString() const{

     stringstream str1;
     str1<< "Watch " ;
     if(getStatus() == COMPLETED){
         str1 <<"COMPLETED";
     }
     if(getStatus() == PENDING){
         str1<<"PENDING";
     }
     if(getStatus() == ERROR){
         str1<< "ERROR: ";
         str1<< getErrorMsg();
     }
     string s = str1.str();
     return s;
}



Watch *Watch::clone() {
    return new Watch(*this);
}

Watch::Watch(Watch &other) :BaseAction(other), id(other.id) , contentWatch(other.contentWatch){
    if(other.getStatus() == COMPLETED){
        complete();
    }
    if(other.getStatus() == ERROR){
        setErrorMsg(other.getErrorMsg());
        error(getErrorMsg());
    }
}

//ACTION PRINT ACTION LOG
PrintActionsLog::PrintActionsLog(){}

void PrintActionsLog::  act(Session& sess){

    for (int i = sess.getActionLog().size()-1; i >=0 ; --i) {
        cout<<  sess.getActionLog()[i]->toString() + "\n";
    }

    complete();
}

string PrintActionsLog::toString() const{

    stringstream str1;
    str1<< "PrintActionLog " ;
    if(getStatus() == COMPLETED){
        str1 <<"COMPLETED";
    }
    if(getStatus() == PENDING){
        str1<<"PENDING";
    }
    if(getStatus() == ERROR){
        str1<< "ERROR: ";
    }
    string s = str1.str();
    return s;
}


PrintActionsLog *PrintActionsLog::clone() {
    return new PrintActionsLog(*this);
}

PrintActionsLog::PrintActionsLog(PrintActionsLog &other) {
    if(other.getStatus() == COMPLETED){
        complete();
    }
}

//ACTION EXIT
Exit::Exit(){}
void Exit:: act(Session& sess){
    complete();
}
string Exit::toString() const{

    stringstream str1;
    str1<< "Exit " ;
    if(getStatus() == COMPLETED){
        str1 <<"COMPLETED";
    }
    if(getStatus() == PENDING){
        str1<<"PENDING";
    }
    if(getStatus() == ERROR){
        str1<< "ERROR: ";
    }
    string s = str1.str();
    return s;
}



Exit *Exit::clone() {
    return new Exit(*this);
}

Exit::Exit(Exit &other) {
    if(other.getStatus() == COMPLETED){
        complete();
    }
}
