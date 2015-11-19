#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <sys/epoll.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
using namespace std;

int main()
{
        struct epoll_event ev;                     //事件临时变量
        const int MAXEVENTS = 1024;                //最大事件数
        struct epoll_event events[MAXEVENTS];      //监听事件数组
        int ret,pid;
        int pipe_fd[2];
        if((ret=pipe(pipe_fd))<0)
        {
                cout<<"create pipe fail:"<<ret<< ",errno:" << errno <<endl;
                return -1;
        }
        ev.data.fd = pipe_fd[0];        //设置监听文件描述符
        ev.events = EPOLLIN|EPOLLET;    //设置要处理的事件类型
        int epfd=epoll_create(MAXEVENTS);
        ret=epoll_ctl(epfd,EPOLL_CTL_ADD,pipe_fd[0],&ev);
        if (ret != 0)
        {
                cout << "epoll_ctl fail:" << ret << ",errno:" << errno << endl;
                close(pipe_fd[0]);
                close(pipe_fd[1]);
                close(epfd);
                return -1;
        }
        if((pid=fork())>0)
        {
                int count=epoll_wait(epfd,events,MAXEVENTS,5000);
                char r_buf[100];
                for(int i=0;i<count;i++)
                {
                        if((events[i].data.fd==pipe_fd[0])&&(events[0].events&EPOLLIN))
                        {
                                int r_num=read(pipe_fd[0],r_buf,100);
                                printf("read num is %d bytes data from the pipe,value is %d \n",r_num,atoi(r_buf));
                        }
                }
                close(pipe_fd[1]);
                close(pipe_fd[0]);
                close(epfd);
                cout<<"parent close read fd[0],wirte fd[1] and epfd over"<<endl;

        }
        else if(pid==0)
        {
                close(pipe_fd[0]);//read
                cout<<"sub close read fd[0] over,sub does't read"<<endl;
                char w_buf[100];
                strcpy(w_buf,"1234");
                if(write(pipe_fd[1],w_buf,5)!=-1)//you can remove this line for learn
                        printf("sub write over \n");
                close(pipe_fd[1]);//write
                printf("sub close write fd[1] over \n");

        }

        return 0;
}


