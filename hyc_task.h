#ifndef HYC_TASK_H
#define HYC_TASK_H

#include <iostream>
#include <set>
#include <string>
#include <errno.h>

using namespace std;

#define BUFF_SIZE 1024


class HycTask;

enum HycEventType
{
    EVENT_STANDBY = 0,      // to master
    EVENT_LINTEN,           // to master // test
    EVENT_RECEIVE,          // to master // test
    EVENT_REMOVE,           // to slave  // test
    EVENT_TIMER,            // to slave
    EVENT_MESSAGE           // to slave
};

// to group
struct StandbyDetail{
    HycTask      *task;
};
struct ListenDetail {
    HycTask        *task;
    unsigned long  s_addr; // ip
    short          nPort;
};
struct ReceiveDetail {
    HycTask        *task;
    int            socket;
};

// task
struct TimerDetail {
    HycTask       *task;
    int           nFlag;
    int           nInterval;
    bool          bRepeat;
};
struct MessageDetail {
    int           nLen;
    char*         sData;
};

union EventDetail {
    StandbyDetail   standbydetail;
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
    string              m_sName;

private:

    HycTask            *m_master;
    set<HycTask*>       m_slavers;
};


#endif
