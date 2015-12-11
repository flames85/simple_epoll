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

// fd操作
//EPOLL_CTL_ADD：注册新的fd到epfd中；
//EPOLL_CTL_MOD：修改已经注册的fd的监听事件；
//EPOLL_CTL_DEL：从epfd中删除一个fd；

// fd工作的几种模式
//EPOLLIN ：表示对应的文件描述符可以读（包括对端SOCKET正常关闭）；

//EPOLLOUT：表示对应的文件描述符可以写；
//EPOLLPRI：表示对应的文件描述符有紧急的数据可读（这里应该表示有带外数据到来）；
//EPOLLERR：表示对应的文件描述符发生错误；
//EPOLLHUP：表示对应的文件描述符被挂断；

//EPOLLET： 将EPOLL设为边缘触发(Edge Triggered-手动挡)模式，这是相对于水平触发(Level Triggered)来说的(默认是LT模式-自动挡)。

//EPOLLONESHOT：只监听一次事件，当监听完这次事件之后，如果还需要继续监听这个socket的话，需要再次把这个socket加入到EPOLL队列里

#define MAX_EVENTS 10
#define PORT 8080

struct EventData{
	int  fd;
	char name[1024];
};

//设置socket连接为非阻塞模式
void setNonBlocking(int sockfd)
{
    int opts;

    opts = fcntl(sockfd, F_GETFL);
    if(opts < 0) {
        perror("fcntl(F_GETFL)n");
        exit(1);
    }
    opts = (opts | O_NONBLOCK);
    if(fcntl(sockfd, F_SETFL, opts) < 0)
    {
        perror("fcntl(F_SETFL)n");
        exit(1);
    }
}

void setReuse(int sockfd)
{
    // 端口复用,为了不至于timewait的端口无法再次监听
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(opt));
}

int main()
{
    struct epoll_event ev, events[MAX_EVENTS];
    int addrlen, listenfd, conn_sock, nfds, epfd, fd, i, nread, n;
    struct sockaddr_in local, remote;
    char buf[BUFSIZ];

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
    // 3. bind
    bzero(&local, sizeof(local));
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = htonl(INADDR_ANY);;
    local.sin_port = htons(PORT);
    if( bind(listenfd, (struct sockaddr *) &local, sizeof(local)) < 0)
    {
        perror("bindn");
        exit(1);
    }

    // 4. listen
    listen(listenfd, 20);

    // 5. 创建epoll-fd
    epfd = epoll_create(MAX_EVENTS);
    if (epfd == -1)
    {
        perror("epoll_create");
        exit(EXIT_FAILURE);
    }

    // 6. 添加listen-fd的可读事件

	EventData *data = new EventData;
	data->fd = listenfd;
	memset(data->name, 0x0, sizeof(data->name));
	strcpy(data->name, "listen");

	ev.data.ptr = data;
    ev.events = EPOLLIN;

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev) == -1)
    {
        perror("epoll_ctl: listen_sock");
        exit(EXIT_FAILURE);
    }

	int connectCount = 0;
    for (;;)
    {
        // 等待
        nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
        if (nfds == -1)
        {
            perror("epoll_pwait");
            exit(EXIT_FAILURE);
        }

        for (i = 0; i < nfds; ++i)
        {
            EventData* data = (EventData*)(events[i].data.ptr);
			int fd = data->fd;
			char* name = data->name;

            if (fd == listenfd)
            {
                while ((conn_sock = accept(listenfd,
                                    (struct sockaddr *) &remote,
                                    (socklen_t*)&addrlen)) > 0)
                {
                    setNonBlocking(conn_sock);
                    // LT自动挡，ET手动挡(epoll)
                    ev.events = EPOLLIN | EPOLLET; // 因为accept成功了，所以设置可读和边缘触发
					EventData *data = new EventData;
					data->fd = conn_sock;
					memset(data->name, 0x0, sizeof(data->name));
					sprintf(data->name, "connect%d", connectCount++);
					ev.data.ptr = data;

                    printf("new connection named[%s]\n", data->name);

                    if (epoll_ctl(epfd, EPOLL_CTL_ADD, conn_sock, &ev) == -1)
                    {
                        perror("epoll_ctl: add");
                        exit(EXIT_FAILURE);
                    }
                }
                if (conn_sock == -1)
                {
                    if (errno != EAGAIN && errno != ECONNABORTED
                        && errno != EPROTO && errno != EINTR)
                    {
                        perror("accept");
                    }
                }
                continue;
            }
            if (events[i].events & EPOLLIN) // 可读
            {
                n = 0;
                while ((nread = read(fd, buf + n, BUFSIZ-1)) > 0)
                {
                    n += nread;
                }
				if(nread == 0) // 正常关闭
				{
					printf("[%s] close\n", data->name);
					close(fd); // close了socket后就不用从，epoll_ctl 中删除了
					continue;
				}
                if (nread == -1 && errno != EAGAIN)
                {
                    perror("read error");
					continue;
                }

				// 到这里肯定收到了数据
				buf[n] = '\0';
				printf("[%s] %s\n", data->name, buf);

				// 回复
				memset(buf, 0x0, sizeof(buf));
                sprintf(buf, "HTTP/1.1 200 OK\r\nContent-Length: 12\r\nHello World\r\n\r\n");
                int nwrite, data_size = strlen(buf);
                n = data_size;
                while (n > 0)
                {
                    nwrite = write(fd, buf + data_size - n, n);
					if (nwrite <= 0 && errno != EAGAIN)
					{
						perror("write error");
						close(fd); // close了socket后就不用从，epoll_ctl 中删除了
						break;
					}
                    n -= nwrite;
                }
            }
        }
    }

    return 0;
}
