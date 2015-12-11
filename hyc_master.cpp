// linux
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/epoll.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
// pro
#include "hyc_master.h"
#include "hyc_slave.h"

#define MAX_EVENTS  10


HycMaster::HycMaster(const string &sName):
    HycThread(sName)
{
    memset(m_pipe_fd, 0x0, sizeof(m_pipe_fd));
}

HycMaster::~HycMaster()
{
}

bool HycMaster::PostEvent(const HycEvent &event)
{
    // 加写锁
    if(write(m_pipe_fd[1], &event, sizeof(event)) <= 0)
        return false;
    return true;
}

bool HycMaster::Event(list<HycEvent> &eventList)
{
    // 加读锁
    bool bExists = false;
    HycEvent event;
    while(read(m_pipe_fd[0], &event, sizeof(event)) >= 0)
    {
        eventList.push_back(event);
        bExists = true;
    }
    return bExists;
}


//设置socket连接为非阻塞模式
bool HycMaster::SetNonBlocking(int sockfd)
{
    int opts;

    opts = fcntl(sockfd, F_GETFL);
    if(opts < 0) {
        perror("fcntl(F_GETFL)n");
        return false;
    }
    opts = (opts | O_NONBLOCK);
    if(fcntl(sockfd, F_SETFL, opts) < 0)
    {
        perror("fcntl(F_SETFL)n");
        return false;
    }
    return true;
}

void HycMaster::SetReuse(int sockfd)
{
    // 端口复用,为了不至于timewait的端口无法再次监听
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(opt));
}

int HycMaster::GetCurrentSecond() {
    return time((time_t*)NULL);
}

bool HycMaster::BindPipe()
{
    if(m_pipe_fd[0] != 0) return false;       // 已经绑定过

    struct epoll_event tmp_event;             //事件临时变量
    int ret = 0;
    if( (ret = pipe(m_pipe_fd)) <0)
    {
        cout << "create pipe fail: " << ret << ",errno:" << errno <<endl;
        return false;
    }

    // 2. 非阻塞
    SetNonBlocking(m_pipe_fd[0]);

    ObjectData *oData = new ObjectData();
    oData->slave = NULL;
    oData->type = FD_TYPE_PIPE;
    tmp_event.data.ptr = oData;
    tmp_event.events = EPOLLIN;    //设置要处理的事件类型

    ret = epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_pipe_fd[0], &tmp_event);
    if (ret != 0)
    {
        cout << "epoll_ctl fail:" << ret << ",errno:" << errno << endl;
        close(m_pipe_fd[0]);
        close(m_pipe_fd[1]);
        return false;
    }
    return true;
}

bool HycMaster::BindListen(HycSlave *slave, unsigned long s_addr, int nPort)
{
    if(!slave) return false;


    // 添加进成员变量里
    if(!SaveSlave(slave))
    {
        return false;
    }

    int listenfd = 0;
    //创建listen socket
    if( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("sockfdn");
        return false;
    }
    // 设置fd属性都在绑定前
    // 1. 端口复用
    SetReuse(listenfd);
    // 2. 非阻塞
    SetNonBlocking(listenfd);

    struct sockaddr_in addr;
    // 3. bind
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = s_addr;
    addr.sin_port = htons(nPort);
    if( bind(listenfd, (struct sockaddr *) &addr, sizeof(addr)) < 0)
    {
        perror("bindn");
        return false;
    }

    // 4. listen
    listen(listenfd, 20);

    slave->m_socket = listenfd;

    // 6. 添加listen-fd的可读事件
    ObjectData *oData = new ObjectData();
    oData->slave = slave;
    oData->type = FD_TYPE_LISTEN;

    // event结构
    struct epoll_event tmp_event;
    tmp_event.events = EPOLLIN;
    tmp_event.data.ptr = oData;
    if (epoll_ctl(m_epfd, EPOLL_CTL_ADD, slave->m_socket, &tmp_event) == -1)
    {
        perror("epoll_ctl: listen_sock");
        return false;
    }

    return true;
}

bool HycMaster::BindReciver(HycSlave *slave, int socket)
{
    if(!slave) return false;
    // 添加进成员变量里
    if(!SaveSlave(slave))
    {
        return false;
    }

    SetNonBlocking(socket);
    slave->m_socket = socket;


    ObjectData *oData = new ObjectData();
    oData->slave = slave;
    oData->type = FD_TYPE_RECIVER;

    // event结构
    struct epoll_event tmp_event;
    tmp_event.events = EPOLLIN | EPOLLET; // 因为accept成功了，所以设置可读和边缘触发
    tmp_event.data.ptr = oData;

    if (epoll_ctl(m_epfd, EPOLL_CTL_ADD, slave->m_socket, &tmp_event) == -1)
    {
        perror("epoll_ctl: add");
        return false;
    }
    return true;
}

// 通过读取的管道信息, 转化而来,或者内部写回
bool HycMaster::BindTimer(const string &sName, int nFlag, int nInterval, bool bRepeat)
{
    TimerInfo *info = new TimerInfo;
    int nTimeout = GetCurrentSecond() + nInterval;

    info->sName = sName;
    info->nFlag = nFlag;
    info->nInterval = nInterval;
    info->nTimeout = nTimeout;
    info->bRepeat = bRepeat;

    m_timerTasks.push_back(info);
    m_timerTasks.sort(TimerInfo());

    cout << "master:" << m_sName << " timer-size:" << m_timerTasks.size() << endl;

    return true;
}

bool HycMaster::SaveSlave(HycSlave *slave)
{
    if(!slave) return false;

    // 添加进成员变量里
    if(m_slaveMap.find(slave->m_sName) == m_slaveMap.end())
    {
        m_slaveMap[slave->m_sName] = slave;

        cout << "master:" << m_sName
             << "slave-size:"
             << m_slaveMap.size()
             << " after add:"
             << slave->m_sName
             << endl;
    }
    else
    {
        cout << "master:"
             << m_sName
             << "slave-size:"
             << m_slaveMap.size()
             << " eixsts:"
             << slave->m_sName
             << endl;
        return false;
    }
    return true;
}

bool HycMaster::DelSlave(const string &sName)
{
    if(m_slaveMap.find(sName) != m_slaveMap.end())
    {
        HycSlave *slave = m_slaveMap[sName];
        m_slaveMap.erase(sName);
        delete slave;

        cout << "master:"
             << m_sName << "slave-size:"
             << m_slaveMap.size()
             << " after erase:"
             << sName
             << " erase ok."
             << endl;
    }
    else
    {
        cout << "master:"
             << m_sName << "slave-size:"
             << m_slaveMap.size()
             << " after erase:"
             << sName
             << " erase fail."
             << endl;
    }

    return true;
}

HycSlave* HycMaster::GetSlave(const string &sName){
    HycSlave* slave = NULL;

    if(m_slaveMap.find(sName) != m_slaveMap.end())
    {
        slave = m_slaveMap[sName];
    }
    return slave;
}

int HycMaster::DoTimer()
{
    int epoll_wait = -1;
    int nDiff = 0;
    while(!m_timerTasks.empty())
    {
        TimerInfo *info = m_timerTasks.front(); // 获取第一条(不删除)

        int nCurrent = GetCurrentSecond();
        nDiff = info->nTimeout - nCurrent;   // 获取epoll_wait最多等待的时间
        if(nDiff <= 0)                          // 假如定时任务已经超时了
        {
            HycSlave *slave = GetSlave(info->sName);
            if(!slave)
            {
                m_timerTasks.pop_front();                   // 不存在了,则直接删除
                delete info;
                cout << "master:" << m_sName << " timer-size:" << m_timerTasks.size() << endl;
                continue;
            }
            slave->TriggerTimeout(info->nFlag);              // 执行
            if(info->bRepeat)                                // 重复
            {
                info->nTimeout = GetCurrentSecond() + info->nInterval;
                m_timerTasks.sort(TimerInfo());
                cout << "master:" << m_sName << " timer-size:" << m_timerTasks.size() << endl;
            }
            else
            {
                m_timerTasks.pop_front();                   // 不重复的,则直接删除
                delete info;
            }
            continue;
        }
        else
        {
            epoll_wait = nDiff;
            break;
        }
    }
    return epoll_wait;
}

int HycMaster::ThreadProc()
{
    // 所有事件
    struct epoll_event all_events[MAX_EVENTS];

    // 5. 创建epoll-fd
    m_epfd = epoll_create(MAX_EVENTS);
    if (m_epfd == -1)
    {
        perror("epoll_create");
        exit(EXIT_FAILURE);
    }

    BindPipe();

    int nfds = 0; // 事件数量
    int epoll_wait_max_time = -1;
    for (;;)
    {
        epoll_wait_max_time = DoTimer();
        nfds = epoll_wait(m_epfd, all_events, MAX_EVENTS, epoll_wait_max_time);

        if(nfds == 0) // 定时器超时的
        {
            continue;
        }
        else if(nfds == -1) // error
        {
            cout << "epoll_pwait error:" << errno << endl;
            perror("epoll_pwait");
            continue;
        }
        for (int i = 0; i < nfds; ++i) // events
        {
            ObjectData *oData = (ObjectData*)(all_events[i].data.ptr);
            uint32_t event_info = all_events[i].events;

            switch(oData->type)
            {
                case FD_TYPE_LISTEN:
                {
                    struct sockaddr_in addr; // 对端地址
                    int addrlen = sizeof(addr); // 对端地址长度
                    int new_conn = accept(oData->slave->m_socket,
                                          (struct sockaddr *) &addr,
                                          (socklen_t*)&addrlen) ;
                    if(new_conn < 0)
                    {
                        if (errno != EAGAIN && errno != ECONNABORTED
                            && errno != EPROTO && errno != EINTR)
                        {
                            perror("accept");
                        }
                        continue;
                    }
                    else
                    {
                        printf("new connection\n");
                        SetNonBlocking(new_conn);

                        HycSlave *slave = NULL;
                        oData->slave->TriggerNewConnection(new_conn, &slave);
                        if(slave != NULL) // 立即绑定receiver
                        {
                            BindReciver(slave,
                                        new_conn);
                        }
                    }
                }
                break;
                case FD_TYPE_RECIVER:
                {
                    if (event_info & EPOLLIN) // 可读
                    {
                        oData->slave->TriggerReadReady();
                    }
                }
                break;
                case FD_TYPE_PIPE:
                {
                    //! 3. 管道事件
                        if(event_info & EPOLLIN)
                    {
                        list<HycEvent> eventList;
                        if(!Event(eventList))
                        {
                            continue; // error
                        }

                        list<HycEvent>::iterator itEvent = eventList.begin();
                        for(; itEvent != eventList.end(); ++ itEvent)
                        {
                            HycEvent &event = (*itEvent);
                            switch(event.type)
                            {
                                case EVENT_STANDBY: //
                                break;

                                case EVENT_LINTEN: //  监听
                                {
                                    BindListen(event.detail.listenDetail.slave,
                                               event.detail.listenDetail.s_addr,
                                               event.detail.listenDetail.nPort);
                                }
                                break;

                                case EVENT_RECEIVE: //  收数据
                                {
                                    BindReciver(event.detail.receiveDetail.slave,
                                                event.detail.receiveDetail.socket);
                                }
                                break;

                                case EVENT_TIMER: //  设定定时器
                                {
                                    BindTimer(event.detail.timerDetail.sName,
                                              event.detail.timerDetail.nFlag,
                                              event.detail.timerDetail.nInterval,
                                              event.detail.timerDetail.bRepeat);
                                }
                                break;

                                case EVENT_REMOVE: // 删除已经登记的slave
                                {
                                    DelSlave(event.detail.removeDetail.sName);
                                }
                                break;

                                case EVENT_MESSAGE: // 对已经登记的slave发消息
                                {
                                    HycSlave *slave = GetSlave(event.detail.messageDetail.sName);

                                    if(slave)
                                    {
                                        slave->TriggerMessage(event.detail.messageDetail.sData);
                                    }
                                    else
                                    {
                                        cout << "master:"
                                             << m_sName
                                             << "not exists:"
                                             << event.detail.messageDetail.sName
                                             << endl;
                                    }
                                }
                                break;

                                default:
                                {

                                }
                                break;
                            }
                        }

                    }
                }
                break;
            default:
                break;
            }
        }
    }
}
