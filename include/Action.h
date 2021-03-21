#ifndef ACTION_H_
#define ACTION_H_

#include <string>
#include <iostream>

class Session;

enum ActionStatus{
    PENDING, COMPLETED, ERROR
};


class BaseAction{
public:
    BaseAction();
    ActionStatus getStatus() const;
    virtual void act(Session& sess)=0;
    virtual std::string toString() const=0;
    virtual BaseAction* clone()=0;
    BaseAction(BaseAction &other);
    virtual ~BaseAction();

protected:
    void complete();
    void error(const std::string& errorMsg);
    std::string getErrorMsg() const;
    void setErrorMsg(std::string s);

private:
    std::string errorMsg;
    ActionStatus status;
};

class CreateUser  : public BaseAction {
public:
    CreateUser(std::string name, std::string algoCode);
    CreateUser(CreateUser &other);
    virtual void act(Session& sess);
    virtual std::string toString() const;
    CreateUser* clone();

private:
    std::string userName;
    std::string code;
};

class ChangeActiveUser : public BaseAction {
public:
    ChangeActiveUser(std::string name);
    ChangeActiveUser(ChangeActiveUser &other);
    virtual void act(Session& sess);
    virtual std::string toString() const;
    ChangeActiveUser* clone();


private:
    std::string userName;

};

class DeleteUser : public BaseAction {
public:
    DeleteUser(std::string name);
    DeleteUser(DeleteUser &other);
    virtual void act(Session & sess);
    virtual std::string toString() const;
    DeleteUser* clone();


private:
    std::string userName;


};


class DuplicateUser : public BaseAction {
public:
    DuplicateUser(std::string original, std::string newName);
    DuplicateUser(DuplicateUser &other);
    virtual void act(Session & sess);
    virtual std::string toString() const;
    DuplicateUser* clone();


private:
    std::string originalUser;
    std::string userName;


};

class PrintContentList : public BaseAction {
public:
    PrintContentList();
    PrintContentList(PrintContentList &other);
    virtual void act (Session& sess);
    virtual std::string toString() const;
    PrintContentList* clone();

};

class PrintWatchHistory : public BaseAction {
public:
    PrintWatchHistory();
    PrintWatchHistory(PrintContentList &other);
    virtual void act (Session& sess);
    virtual std::string toString() const;
    PrintWatchHistory* clone();


};


class Watch : public BaseAction {
public:
    Watch(long id);
    Watch(Watch &other);
    virtual void act(Session& sess);
    virtual std::string toString() const;
    Watch* clone();
private:
    std::string contentWatch;
    long id;

};


class PrintActionsLog : public BaseAction {
public:
    PrintActionsLog();
    PrintActionsLog(PrintActionsLog &other);
    virtual void act(Session& sess);
    virtual std::string toString() const;
    PrintActionsLog* clone();


};

class Exit : public BaseAction {
public:
    Exit();
    Exit(Exit &other);
    virtual void act(Session& sess);
    virtual std::string toString() const;
    Exit* clone();

};
#endif