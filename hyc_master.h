#ifndef HYC_MASTER_H
#define HYC_MASTER_H

// std
#include <string>
#include <map>
#include <set>
#include <list>

// pro
#include "hyc_thread.h"


using namespace std;

class HycSlave;
class HycEvent;
class HycMaster : public HycThread
{
public:
    HycMaster(const string &sName);
    virtual ~HycMaster();

    // 发消息
    virtual bool PostEvent(const HycEvent &event);
    virtual bool Event(list<HycEvent> &eventList);

protected:

    HycSlave* GetSlave(const string &sName);

private:
    bool BindPipe();
    bool BindListen(HycSlave *task, unsigned long s_addr, int nPort);
    bool BindReciver(HycSlave *task, int socket);
    bool BindTimer(const string &sName, int nFlag, int nInterval, bool bRepeat);

    bool SaveSlave(HycSlave *slave);
    bool DelSlave(const string &sName);

    int DoTimer();


    // 线程循环
    virtual int ThreadProc();

    bool SetNonBlocking(int sockfd);
    void SetReuse(int sockfd);
    int  GetCurrentSecond();

public:
    string              m_sName;

private:
    // pipe
    int                 m_pipe_fd[2];

    // epoll fd
    int                 m_epfd;

    //! epoll万能指针枚举值和结构
    enum ObjectFdType {
        FD_TYPE_LISTEN = 0, // to slave
        FD_TYPE_RECIVER,    // to slave
        FD_TYPE_PIPE        // to master
    };
    struct ObjectData
    {
        explicit ObjectData()
        {
            type = FD_TYPE_LISTEN;
            slave = NULL;
        }
        ObjectFdType   type;
        HycSlave       *slave;
    };
    // 记录已经存放进epoll里的名称-对象
    map<string, HycSlave*>     m_slaveMap;



    // 所有在master中的task都在这里保存(当然也包含下面那些timer)
    map<HycSlave*, set<int> >  m_EventTasks;
    // 所有timer
    struct TimerInfo{
        string   sName;
        int      nFlag;         // 定时器的类型
        int      nInterval;     // sec
        int      nTimeout;      // sec
        bool     bRepeat;       // 是否单次

        bool operator()(const TimerInfo* t1, const TimerInfo* t2) // 容器中是指针
        {
            return t1->nTimeout < t2->nTimeout;
        }
        const bool operator < (const TimerInfo* info) const // 容器中是对象
        {
           return (nTimeout <  info->nTimeout);
        }
    };
    list<TimerInfo*>             m_timerTasks;

};

#endif // HYC_MASTER_H
