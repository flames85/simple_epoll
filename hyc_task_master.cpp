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
#include "hyc_task_master.h"


enum ObjectFdType {
    FD_TYPE_LISTEN = 0,
    FD_TYPE_RECIVER,
    FD_TYPE_PIPE
};

struct ObjectData {
    int            fd;
    HycTask       *task;
    ObjectFdType   type;
    explicit ObjectData()
    {
        fd = 0;
        obj = NULL;
        type = FD_TYPE_LISTEN;
    }
};

HycTaskMaster::HycTaskMaster()
{
}

HycTaskMaster::~HycTaskMaster()
{
}

int GetCurrentSecond() {
    return time((time_t*)NULL);
}

bool HycTaskMaster::Event(const HycEvent &event)
{

}

bool HycTaskMaster::BindPipe(HycTask *task)
{
    struct epoll_event tmp_event;             //事件临时变量
    if((ret = pipe(task->m_pipe_fd)) <0)
    {
        cout << "create pipe fail: " << ret << ",errno:" << errno <<endl;
        return false;
    }

    ObjectData *oData = new ObjectData();
    oData->fd = task->m_pipe_fd[0];
    oData->task = task;
    oData->type = FD_TYPE_PIPE;
    tmp_event.data.ptr = oData;
    tmp_event.events = EPOLLIN | EPOLLET;    //设置要处理的事件类型

    ret = epoll_ctl(m_epfd, EPOLL_CTL_ADD, task->m_pipe_fd[0], &tmp_event);
    if (ret != 0)
    {
        cout << "epoll_ctl fail:" << ret << ",errno:" << errno << endl;
        close(task->m_pipe_fd[0]);
        close(task->m_pipe_fd[1]);
        return false;
    }

    // 添加进成员变量里
    SaveEvent(task, task->m_pipe_fd[0]);

    return true;
}

bool HycTaskMaster::BindListen(HycTask *task, unsigned long s_addr, int nPort)
{
    int listenfd;
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

    //创建listen socket
    if( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("sockfdn");
        exit(1);
    }
    // 设置fd属性都在绑定前
    // 1. 端口复用
    setReuse(listenfd);
    // 2. 非阻塞
    setNonBlocking(listenfd);

    // 6. 添加listen-fd的可读事件
    tmp_event.events = EPOLLIN;
    ObjectData *oData = new ObjectData();
    oData->fd = listenfd;
    oData->task = task;
    oData->type = FD_TYPE_LISTEN;
    tmp_event.data.ptr = oData;

    if (epoll_ctl(m_epfd, EPOLL_CTL_ADD, oData->fd, &tmp_event) == -1)
    {
        perror("epoll_ctl: listen_sock");
        return false;
    }

    // 添加进成员变量里
    SaveEvent(task, oData->fd);
}

bool HycTaskMaster::BindReciver(HycTask *task, int socket)
{
    // 一个临时envnt结构
    struct epoll_event tmp_event;

    setNonBlocking(socket);
    // LT自动挡，ET手动挡(epoll)
    tmp_event.events = EPOLLIN | EPOLLET; // 因为accept成功了，所以设置可读和边缘触发

    ObjectData *oData = new ObjectData();
    oData->fd = socket;
    oData->task = task;
    oData->type = FD_TYPE_RECIVER;
    tmp_event.data.ptr = oData;

    if (epoll_ctl(m_epfd, EPOLL_CTL_ADD, oData->fd, &tmp_event) == -1)
    {
        perror("epoll_ctl: add");
        return false;
    }
    // 添加进成员变量里
    SaveEvent(task, oData->fd);
}

// 通过读取的管道信息, 转化而来,或者内部写回
bool HycTaskMaster::BindTimer(HycTask *task, int nFlag, int nInterval, bool bRepeat)
{
    if(!obj) return false;

    TimerInfo info;
    int nTimeout = GetCurrentSecond() + nInterval;

    info.task = task;
    info.nFlag = nFlag;
    info.nInterval = nInterval;
    info.nTimeout = nTimeout;
    info.bRepeat = bRepeat;

    list<TimerInfo>::iterator it = m_timerTasks.begin();

    for(; it != m_timerTasks.end(); it++)
    {
        if(it->nTimeout >= info.nTimeout) {
            m_timerTasks.insert(it, info);
        }
    }
    return true;
}

bool HycTaskMaster::RemoveBind(HycTask *task)
{
    DelEvent(task);
    delete info.task;
    return true;
}

void HycTaskMaster::SaveEvent(HycTask *task, int fd)
{
    // 添加进成员变量里
    if(m_EventTasks.find(task) == m_EventTasks.end())
    {
        list<int> fdList;
        fdList.push_back(task->m_pipe_fd[0]);
        m_EventTasks.insert(task, fdList);
    }
    else
    {
        m_EventTasks[task].push_back(fd);
    }
}

void HycTaskMaster::DelEvent(HycTask *task)
{
    map<HycTask*, list<int> >::iterator itTask = m_EventTasks.find(task);
    if(itTask == m_EventTasks.end())
    {
        return true;
    }
    const list<int> &fdList = it->second();

    list<int>::const_iterator itfd =fdList.begin();

    for(; itfd != fdList.end(); itfd++)
    {
        int socket = *itfd;
        if (epoll_ctl(m_epfd, EPOLL_CTL_DEL, socket, NULL) == -1)
        {
            perror("epoll_ctl: EPOLL_CTL_DEL");
            return false;
        }
    }
}


int HycTaskMaster::ThreadProc()
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

    BindPipe(this);

    int nfds = 0; // 事件数量
    for (;;)
    {
        if(m_timerTasks.empty()) // 当前无定时器任务, 无限等待
        {
            nfds = epoll_wait(m_epfd, all_events, MAX_EVENTS, -1);
        }
        else // 有定时器任务, 获取最早会超时的任务
        {
            TimerInfo info = m_timerTasks.front(); // 获取第一条(不删除)
            int nCurrent = GetCurrentSecond();
            int epill_wait_max_time = info.nTimeout - nCurrent; // 获取epoll_wait最多等待的时间
            if(epill_wait_max_time <= 0) // 假如定时任务已经超时了
            {
                m_timerTasks.pop_front(); // 可以把第一条删了
                info.task->TriggerTimeout(info.flag);         // 执行
                if(!info.bRepeat) // 不是只执行一次的就继续添加
                    BindTimer(info.task, info.nFlag, info.nInterval, info.bRepeat);
            }
            else
            {
                nfds = epoll_wait(m_epfd, all_events, MAX_EVENTS, epill_wait_max_time*1000);
            }
        }

        if(nfds == 0) // 定时器超时的
        {
            TimerInfo info = m_timerTasks.pop_front(); // 确定是超时导致的, 可以把第一条删了

            if(m_EventTasks.find(info.task) == m_EventTasks.end())
            {
                cout << "task has been deleted" << endl;
                continue;
            }

            info.task->TriggerTimeout(info.flag);              // 执行
            if(!info.bRepeat) // 不是只执行一次的就继续添加
                BindTimer(info.task, info.nFlag, info.nInterval, info.bRepeat);      // 继续添加
        }
        else if(nfds == -1) // error
        {
            perror("epoll_pwait");
            continue;
        }
        for (i = 0; i < nfds; ++i) // events
        {
            ObjectData *oData = (ObjectData*)(all_events[i].data.ptr);
            event_info = all_events[i].events;

            switch(oData->type)
            {
                case FD_TYPE_LISTEN:
                {
                    struct sockaddr_in addr; // 对端地址
                    int addrlen; // 对端地址长度
                    int new_conn = accept(oData->fd,
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
                        setNonBlocking(new_conn);
                        oData->task->TriggerNewConnection(new_conn); // 期望回调 AddTcpReceiver
                    }
                }
                break;
                case FD_TYPE_RECIVER:
                {
                    if (event_info & EPOLLIN) // 可读
                    {
                        if(nTotal > 0)
                        {
                            buf[n] = '\0';
                            printf("recv:%s\n", buf);
                            oData->task->TriggerReadReady(oData->fd);
                        }
                    }
                }
                break;
                case FD_TYPE_PIPE:
                {
                    //! 3. 管道事件
                    if(event_info & EPOLLIN)
                    {
                        HycEvent event;
                        if(info.task == this) // 1. 管道消息是发给master的
                        {
                            if(!info.task->Event(event))
                            {
                                // error
                                continue;
                            }
                            switch(event.type)
                            {
                                case EVENT_REGISTER:
                                // 1. 管道注册
                                BindPipe(event.detail.registerdetail.task);
                                break;
                                default:
                                break;
                            }
                        }
                        else                  // 2. 管道已经注册过,发的是进一步事件
                        {
                            if(!info.task->Event(event))
                            {
                                // error
                                continue;
                            }
                            switch(event.type)
                            {
                                case EVENT_LINTEN:
                                BindListen(info.task,
                                           event.detail.listenDetail.s_addr,
                                           event.detail.listenDetail.nPort);
                                break;

                                case EVENT_RECEIVE:
                                BindReciver(info.task, event.detail.receiveDetail.socket);
                                break;

                                case EVENT_TIMER:
                                BindTimer(info.task,
                                          event.detail.timerDetail.nFlag,
                                          event.detail.timerDetail.nInterval,
                                          event.detail.timerDetail.bRepeat);
                                BindPipe(info.task);
                                break;

                                case EVENT_MESSAGE:
                                info.task->TriggerMessage(event.detail.messageDetail.sData,
                                                          event.detail.messageDetail.nLen);
                                break;

                                case EVENT_REMOVE:

                                RemoveBind(info.task);

                                break;

                                default:
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