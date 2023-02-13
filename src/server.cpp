#ifndef SERVER_H
#define SERVER_H

#include "include.h"
#include "Mysql.h"
#include "logIn_Register.h"

#define USER_LIMIT 5
#define FD_LIMIT 65535

int create_listen_socket(const char* ip, int port) {
    // set address
    sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);

    // create listen socket
    int listen_socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    int on = 1;

    if (setsockopt(listen_socket_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1) {
        printf("setsockopt error!\n");
        return -1;
    }

    if (bind(listen_socket_fd, (sockaddr*)&address, sizeof(address)) == -1) {
        printf("bind error!\n");
        return -1;
    }

    if (listen(listen_socket_fd, 5) == -1) {
        printf("set listen socket error!\n");
        return -1;
    }

    return listen_socket_fd;
}

int create_connection_socket(int listen_socket_fd, std::vector<int>& users_fd, 
                             std::map<int, Networks::client_data*>& users, int epollfd) {
    sockaddr_in client_address;
    socklen_t client_addrlength = sizeof(client_address);
    int connection_socket_fd = accept(listen_socket_fd, (sockaddr*)&client_address, &client_addrlength);

    if (connection_socket_fd < 0) {
        printf("errno is: %d\n", errno);
        return -1;
    }

    // if client quantity overflow
    if (users_fd.size() >= USER_LIMIT) {
        const char* info = "too many users\n";
        printf("%s", info);
        send(connection_socket_fd, info, strlen(info), 0);
        close(connection_socket_fd);
        return -1;
    }

    // add this new connection_socket_fd into epoll tree and array of users
    users_fd.push_back(connection_socket_fd);
    users[connection_socket_fd] = new Networks::client_data();
    users[connection_socket_fd]->address = client_address;
    users[connection_socket_fd]->fds_index = users_fd.size() - 1;

    Networks::addfd(epollfd, connection_socket_fd, true);

    // add readable event
    epoll_event event;

    event.data.fd = connection_socket_fd;
    event.events = EPOLLERR | EPOLLRDHUP;
    event.events |= EPOLLOUT;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, connection_socket_fd, &event);

    return connection_socket_fd;
}

void delete_connection_socket(int socket_fd, std::vector<int>& users_fd, 
                              std::map<int, Networks::client_data*>& users, int epollfd) {
    int t = users[socket_fd]->fds_index;
    std::swap(users_fd[t], users_fd.back());
    users[users_fd[t]]->fds_index = t;

    users_fd.pop_back();
    close(socket_fd);
}

void send_message (std::string& message, int user_fd, 
                   std::map<int, Networks::client_data*>& users, int epollfd) {
    // Trigger writable events of other user sockets
    epoll_event event;

    event.data.fd = user_fd;
    event.events = EPOLLERR | EPOLLRDHUP;
    event.events |= EPOLLOUT;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, user_fd, &event);

    memcpy(users[user_fd]->buf, message.c_str(), sizeof(char) * message.size());
    users[user_fd]->buf[message.size()] = '\0';
}

void group_message (std::string& message, std::vector<int>& users_fd, 
                    std::map<int, Networks::client_data*>& users, int epollfd) {
    int user_count = users_fd.size();

    for (int i = 0; i < user_count; i++) {
        // Trigger writable events of other user sockets
        epoll_event event;

        event.data.fd = users_fd[i];
        event.events = EPOLLERR | EPOLLRDHUP;
        event.events |= EPOLLOUT;
        epoll_ctl(epollfd, EPOLL_CTL_MOD, users_fd[i], &event);

        memcpy(users[users_fd[i]]->buf, message.c_str(), sizeof(char) * message.size());
        users[users_fd[i]]->buf[message.size()] = '\0';
    }
}

void log_in(MYSQL& com_mysql, int connection_socket_fd, std::vector<int>& users_fd, 
            std::map<int, Networks::client_data*>& users, int epollfd) {
    std::string id_key = users[connection_socket_fd]->buf, key, name, splits = " ";
    std::vector<std::string> id_key_vector;
    int id = 0;

    Networks::string_split(id_key, splits, id_key_vector);
    std::string message_tmp;

    for (int i=0;i<id_key_vector.size(); i++) {
        if (id == 0 && 
            id_key_vector[i].size() != 0) {
            id = atoi(id_key_vector[i].c_str());
        }
        else if(id_key_vector[i].size() != 0) {
            key = id_key_vector[i];
            break;
        }
    }
    
    if(Networks::check_user_key(com_mysql, id, key, name)) {
        users[connection_socket_fd]->id = id;
        users[connection_socket_fd]->name = name;

        message_tmp = "Welcome back, " + name + "!\n";
    } else {
        message_tmp = "Wrong ID or KEY!\n";
    }

    memcpy(users[connection_socket_fd]->buf, message_tmp.c_str(), sizeof(char) * message_tmp.size());
    users[connection_socket_fd]->buf[message_tmp.size()] = '\0';
    send_message(message_tmp, connection_socket_fd, users, epollfd);
}

int main(int argc, char* argv[]) {
    // input pareameters
    if (argc < 2) {
        printf("usage: %s  ip_address port_number\n", basename(argv[0]));
        return 1;
    }
    
    const char* ip = argv[1];
    int port = atoi(argv[2]);

    // user structure, events of epoll
    std::map<int, Networks::client_data*> users;
    std::vector<int> users_fd;
    int epollfd = epoll_create(USER_LIMIT + 1);
    epoll_event events[USER_LIMIT + 1];

    // mysql initial
    MYSQL com_mysql = Networks::mysql_init();

    // add listen_socket_fd into epoll tree
    int listen_socket_fd = create_listen_socket(ip, port);
    if(listen_socket_fd == -1)
        return -1;
    Networks::addfd(epollfd, listen_socket_fd, false);

    // work
    while (true) {
        int number = epoll_wait(epollfd, events, USER_LIMIT + 1, -1);

        if (number < 0 && errno != EINTR) {
            printf("epoll failture\n");
            break;
        }

        for (int i = 0; i < number; ++i) {
            // for socket of each events
            if (events[i].data.fd == listen_socket_fd) {
                // event of listen socket, mean a new connection  
                int connection_socket_fd = create_connection_socket(listen_socket_fd, users_fd, users, epollfd);

                if(connection_socket_fd == -1)
                    continue;

                printf("comes a new user, now have %d users\n", (int)users_fd.size());

                // ask the id and key of this connection client
                std::string message_tmp = "Your id and key is ?\n";
                memcpy(users[connection_socket_fd]->buf, message_tmp.c_str(), sizeof(char) * message_tmp.size());
                users[connection_socket_fd]->buf[message_tmp.size()] = '\0';
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
                delete_connection_socket(events[i].data.fd, users_fd, users, epollfd);
                printf("a client left\n");
            }
            else if (events[i].events & EPOLLIN) {
                // a socket sends me some messages, reading it 
                int connection_socket_fd = events[i].data.fd;

                memset(users[connection_socket_fd]->buf, '\0', BUFFER_SIZE);
                int ret = recv(
                    connection_socket_fd, 
                    users[connection_socket_fd]->buf, 
                    BUFFER_SIZE - 1, 0
                    );

                printf("get %d bytes of client (%d) data: %s\n", ret, connection_socket_fd, users[connection_socket_fd]->buf);

                if (ret < 0) {
                    if (errno != EAGAIN) 
                        delete_connection_socket(events[i].data.fd, users_fd, users, epollfd);
                }
                else if (ret > 0) {
                    // get the id and key of clients
                    if (users[connection_socket_fd]->id == 0) {
                        log_in(com_mysql, connection_socket_fd, users_fd, users, epollfd);
                        continue;
                    }

                    // send group message
                    std::string message = "User " + std::to_string(connection_socket_fd) + 
                                          ": " + users[connection_socket_fd]->buf;
                    group_message(message, users_fd, users, epollfd);
                }
            }
            else if (events[i].events & EPOLLOUT) {
                // Triggered by the writable event of the mass user
                int connection_socket_fd = events[i].data.fd;
                int ret = send(
                    connection_socket_fd, 
                    users[connection_socket_fd]->buf, 
                    strlen(users[connection_socket_fd]->buf), 0
                    );

                // Re-register the readable event and mask the read event on the current socket during the sending process
                epoll_event event;
                event.data.fd = connection_socket_fd;
                event.events = EPOLLERR | EPOLLRDHUP;
                event.events |= EPOLLIN;
                epoll_ctl(epollfd, EPOLL_CTL_MOD, connection_socket_fd, &event);
            }
            else
                printf("something else happend \n");
        }
    }

    close(listen_socket_fd);
    close(epollfd);

    mysql_close(&com_mysql);//善后处理

    return 0;
}

#endif