#ifndef SERVER_H
#define SERVER_H

#include "application.h"
#include "basic.h"
#include "include.h"
#include "logIn_register.h"
#include "Mysql.h"


int main(int argc, char* argv[]) {
    // input pareameters
    if (argc < 2) {
        printf("usage: %s ip_address port_number\n", basename(argv[0]));
        return 1;
    }
    const char* ip = argv[1];
    int port = atoi(argv[2]);

    // initial mysql, thread pool
    MYSQL m_mysql = Networks::mysql_init();

    ThreadPool threadPool;
    threadPool.init();

    // add listen_socket_fd into epoll tree
    int listen_socket_fd = Networks::create_listen_socket(ip, port);
    if (listen_socket_fd == -1)
        return -1;

    map<int, Networks::ClientData*> users_data;
    vector<int> users_fd;

    int epoll_fd = epoll_create(USER_LIMIT + 1);
    epoll_event events[USER_LIMIT + 1];
    Networks::addfd(epoll_fd, listen_socket_fd, false);

    // work
    while (true) {
        int number = epoll_wait(epoll_fd, events, USER_LIMIT + 1, -1);

        if (number < 0 && errno != EINTR) {
            printf("epoll failture\n");
            break;
        }

        for (int i = 0; i < number; ++i) {
            // for socket of each events
            if (events[i].data.fd == listen_socket_fd) {
                // event of listen socket, mean a new connection  
                int connection_socket_fd = create_connection_socket(listen_socket_fd, users_fd, users_data, epoll_fd);
                if (connection_socket_fd != -1)
                    printf("comes a new user, now have %d users\n", (int)users_fd.size());
            }
            else if (events[i].events & EPOLLERR) {
                // a socket appears errors
                printf("get an error from %d\n", events[i].data.fd);
                char errors[100];
                memset(errors, '\0', 100);
                socklen_t length = sizeof(errors);

                // obtain and clear the error state of socket 
                if (getsockopt(events[i].data.fd, SOL_SOCKET, SO_ERROR, &errors, &length) < 0)
                    printf("get socket option failed\n");
                continue;
            }
            else if (events[i].events & EPOLLRDHUP) {
                // a socket left
                delete_connection_socket(events[i].data.fd, users_fd, users_data, epoll_fd);
                printf("a client left\n");
            }
            else if (events[i].events & EPOLLIN) {
                // a socket sends me some messages, reading it 
                int connection_socket_fd = events[i].data.fd;

                char buf[BUFFER_SIZE];
                memset(buf, '\0', BUFFER_SIZE);

                int ret = recv(
                    connection_socket_fd,
                    buf,
                    BUFFER_SIZE - 1, 0
                );

                users_data[connection_socket_fd]->buf = buf;

                if (ret < 0) {
                    if (errno != EAGAIN)
                        delete_connection_socket(events[i].data.fd, users_fd, users_data, epoll_fd);
                }
                else if (ret > 0) {
                    string rev_str = users_data[connection_socket_fd]->buf;

                    printf("get %d bytes of client (%d) data: %s\n", ret, connection_socket_fd, users_data[connection_socket_fd]->buf.c_str());

                    threadPool.addTask(
                        Networks::app_, 
                        users_data[connection_socket_fd]->buf, 
                        connection_socket_fd, 
                        users_data[connection_socket_fd], 
                        epoll_fd);
                    printf(">>>> %s\n", users_data[connection_socket_fd]->buf.c_str());
                }
            }
            else if (events[i].events & EPOLLOUT) {
                // Triggered by the writable event of the mass user
                int connection_socket_fd = events[i].data.fd;
                int cur = 0, n = users_data[connection_socket_fd]->buf.size();
                printf(">> %s\n", users_data[connection_socket_fd]->buf.c_str());
                while(cur < n) {
                    string s = users_data[connection_socket_fd]->buf.substr(cur, min(n - cur, 65535));

                    int ret = send(
                        connection_socket_fd,
                        s.c_str(),
                        s.size(), 0
                    );
                    cur += 65535;
                }

                // Re-register the readable event and mask the read event on the current socket during the sending process
                epoll_event event;
                event.data.fd = connection_socket_fd;
                event.events = EPOLLERR | EPOLLRDHUP;
                event.events |= EPOLLIN;
                epoll_ctl(epoll_fd, EPOLL_CTL_MOD, connection_socket_fd, &event);
            }
            else
                printf("something else happend \n");
        }
    }

    close(listen_socket_fd);
    close(epoll_fd);

    mysql_close(&m_mysql);

    return 0;
}
#endif