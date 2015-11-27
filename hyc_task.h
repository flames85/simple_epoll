#ifndef HYC_TASK_H
#define HYC_TASK_H

#include <iostream>
#include <set>
#include <string>
#include <errno.h>

using namespace std;

#define BUFF_SIZE 1024


class HycTask;

enum HycEventType{
    EVENT_REGISTER = 0, //
    EVENT_LINTEN,       //
    EVENT_RECEIVE,      //
    EVENT_REMOVE,       //
    EVENT_TIMER,
    EVENT_MESSAGE
};

// to group
struct RegisterDetail{
    HycTask      *task;
};

// to Task
struct ListenDetail {
    unsigned long  s_addr; // ip
    short          nPort;
};
struct ReceiveDetail {
    int           socket;
};
struct RemoveDetail {

};
struct TimerDetail {
    int           nFlag;
    int           nInterval;
    bool          bRepeat;
};
struct MessageDetail {
    int           nLen;
    char*         sData;
};

union EventDetail {
    RegisterDetail  registerdetail;
    ListenDetail    listenDetail;
    ReceiveDetail   receiveDetail;
    TimerDetail     timerDetail;
    MessageDetail   messageDetail;
};

struct HycEvent {
    HycEventType type;
    EventDetail  detail;
};

class HycTask
{
public:
    HycTask(const string &sName);
    virtual ~HycTask();

    // 发消息
    virtual bool PostEvent(const HycEvent &event);

    virtual void TriggerNewConnection(int socket);
    virtual void TriggerReadReady(int socket);
    virtual void TriggerTimeout(int nType);
    virtual void TriggerMessage(char* sData, int nLen);
    virtual bool Event(HycEvent &event);

protected:

public:
    int                 m_pipe_fd[2];

private:
    string              m_sName;
    HycTask            *m_master;
    set<HycTask*>       m_slavers;
};


#endif
