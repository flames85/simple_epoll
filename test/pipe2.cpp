#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <sys/epoll.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
using namespace std;

struct Data{
	char** 	name;
	int 	id;
};


int main()
{
    struct epoll_event ev;             //事件临时变量
    const int MAXEVENTS = 1024;        //最大事件数
    struct epoll_event events[MAXEVENTS];      //监听事件数组
    int ret, pid;
    int pipe_fd[2];
    if((ret = pipe(pipe_fd)) <0)
    {
        cout << "create pipe fail: " << ret << ",errno:" << errno <<endl;
        return -1;
    }
    ev.data.fd = pipe_fd[0];      //设置监听文件描述符
    ev.events = EPOLLIN | EPOLLET;    //设置要处理的事件类型
    int epfd = epoll_create(MAXEVENTS);
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, pipe_fd[0], &ev);
    if (ret != 0)
    {
        cout << "epoll_ctl fail:" << ret << ",errno:" << errno << endl;
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        close(epfd);
        return -1;
    }

    pid = fork();
    if(pid < 0)
    {
        cout << "epoll_ctl fail:" << ret << ",errno:" << errno << endl;
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        close(epfd);
        return -1;
    }
    else if(pid > 0) // 子进程
    {
        // 关闭读fd.
        close(pipe_fd[0]);
        cout << "sub: close read fd[0] over, sub does't read" << endl;

        // 写
		Data *data1 = new Data;
		char *name = new char[1024];
		memcpy(name, "Hello", 1024);
		data1->name = &name;
		data1->id = 300;
		cout << "WRITE1:" << *(data1->name) << " " << data1->id << endl;
		printf("addr:%p\n", data1->name);

		Data *data2 = new Data;
		memcpy(data2, data1, sizeof(Data));
		cout << "WRITE2:" << *(data2->name) << " " << data2->id << endl;
		printf("addr:%p\n", data2->name);

        if(write(pipe_fd[1], data1, sizeof(Data)) != -1)
        	printf("sub: write over \n");

        // 关闭写fd
        close(pipe_fd[1]);//write
        printf("sub: close write fd[1] over \n");

    }
    else // 主进程
    {
        int count = epoll_wait(epfd, events, MAXEVENTS, 5000);
        cout << "main: events count is:" << count << endl;

        for(int i = 0; i < count; i++)
        {
			if((events[i].data.fd == pipe_fd[0]) && (events[0].events & EPOLLIN))
			{
				Data *r_data = new Data;
				int r_num = read(pipe_fd[0], r_data, sizeof(Data));
				cout << "READ:" << *(r_data->name) << " " << r_data->id << endl;
				printf("addr:%p\n", r_data->name);
			}
        }
        close(pipe_fd[1]);
        close(pipe_fd[0]);
        close(epfd);
        cout << "main: parent close all" << endl;
    }

    return 0;
}


