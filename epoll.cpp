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

#define MAX_EVENTS 10
#define PORT 8080

//设置socket连接为非阻塞模式
void setnonblocking(int sockfd) 
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

void setreuse(int sockfd)
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
    setreuse(listenfd);
    // 2. 非阻塞
    setnonblocking(listenfd);

    bzero(&local, sizeof(local));
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = htonl(INADDR_ANY);;
    local.sin_port = htons(PORT);
    if( bind(listenfd, (struct sockaddr *) &local, sizeof(local)) < 0)
    {
        perror("bindn");
        exit(1);
    }

    listen(listenfd, 20);

    epfd = epoll_create(MAX_EVENTS);
    if (epfd == -1)
    {
        perror("epoll_create");
        exit(EXIT_FAILURE);
    }

    ev.events = EPOLLIN;
    ev.data.fd = listenfd;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev) == -1)
    {
        perror("epoll_ctl: listen_sock");
        exit(EXIT_FAILURE);
    }

    for (;;)
    {
        nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
        if (nfds == -1)
        {
            perror("epoll_pwait");
            exit(EXIT_FAILURE);
        }

        for (i = 0; i < nfds; ++i)
        {
            fd = events[i].data.fd;
            if (fd == listenfd)
            {
                while ((conn_sock = accept(listenfd,
                                    (struct sockaddr *) &remote,
                                    (socklen_t*)&addrlen)) > 0)
                {
                    printf("new connection\n");
                    setnonblocking(conn_sock);
                    // LT自动挡，ET手动挡(epoll)
                    ev.events = EPOLLIN | EPOLLET; // 因为accept成功了，所以设置可读和边缘触发
                    ev.data.fd = conn_sock;
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
                if (nread == -1 && errno != EAGAIN)
                {
                    perror("read error");
                }
                ev.data.fd = fd;
                ev.events = events[i].events | EPOLLOUT; // 因为捕获到可读了，所以设置可写和边缘触发
                if (epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev) == -1)
                {
                    perror("epoll_ctl: mod");
                }

                if(n > 0)
                {
                    buf[n] = '\0';
                    printf("recv:%s\n", buf);
                }
            }
            if (events[i].events & EPOLLOUT) // 可写
            {
                sprintf(buf, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\nHello World", 11);
                int nwrite, data_size = strlen(buf);
                n = data_size;
                while (n > 0)
                {
                    nwrite = write(fd, buf + data_size - n, n);
                    if (nwrite < n)
                    {
                        if (nwrite == -1 && errno != EAGAIN)
                        {
                            perror("write error");
                        }
                        break;
                    }
                    n -= nwrite;
                }
                close(fd); // close了socket后就不用从，epoll_ctl 中删除了
                printf("bye\n");
            }
        }
    }

    return 0;
}
