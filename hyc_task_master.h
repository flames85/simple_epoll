#ifndef HycTaskMaster_H
#define HycTaskMaster_H

// std
#include <string>
#include <map>
#include <list>

// pro
#include "hyc_thread.h"
#include "hyc_task.h"

using namespace std;


class HycTaskMaster : public HycTask , public HycThread
{
public:
    HycTaskMaster(const string &sName);
    virtual ~HycTaskMaster();

    void test()
    {

    }
private:
    bool BindPipe(HycTask *task);
    bool BindListen(HycTask *task, unsigned long s_addr, int nPort);
    bool BindReciver(HycTask *task, int socket);
    bool BindTimer(HycTask *task, int nFlag, int nInterval, bool bRepeat);
    bool RemoveBind(HycTask *task);

    bool SaveEvent(HycTask *task, int fd);
    bool DelEvent(HycTask *task);

    // 线程循环
    virtual int ThreadProc();

    bool SetNonBlocking(int sockfd);
    void SetReuse(int sockfd);
    int  GetCurrentSecond();

private:
    // epoll fd
    int                 m_epfd;

    // 所有在master中的task都在这里保存(当然也包含下面那些timer)
    map<HycTask*, set<int> >  m_EventTasks;


    // 所有timer
    struct TimerInfo{
        HycTask* task;
        int      nFlag;         // 定时器的类型
        int      nInterval;     // sec
        int      nTimeout;      // sec
        bool     bRepeat;       // 是否单次
    };
    list<TimerInfo>             m_timerTasks;
};

#endif // HycTaskMaster_H
