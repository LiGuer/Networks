#ifndef INCLUDE_H
#define INCLUDE_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <string>
#include <fcntl.h>
#include <libgen.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <stdbool.h>
#include <vector>
#include <map>

#define BUFFER_SIZE 1024
#define MYSQL_IP "127.0.0.1"

namespace Networks {

    struct client_data {
        int fds_index;
        sockaddr_in address;
        int id = 0;
        std::string name;

        char buf[BUFFER_SIZE];
    };

    void string_split(std::string str, std::string& splits, std::vector<std::string>& answer) {
        if (str == "")
            return;

        size_t pos = str.find(splits);
        int step = splits.size();
    
        while (pos != str.npos) {
            answer.push_back(str.substr(0, pos));
            str = str.substr(pos + step, str.size());
            pos = str.find(splits);
        }
        if (str != "")
            answer.push_back(str);
    }

    int setnonblocking(int fd) {
        int old_option = fcntl(fd, F_GETFL),
            new_option = old_option | O_NONBLOCK;

        fcntl(fd, F_SETFL, new_option);
        return old_option;
    }

    void addfd(int epollfd, int fd, bool hup) {
        epoll_event event;
        event.data.fd = fd;
        event.events = EPOLLIN | EPOLLERR;

        if (hup) {
            event.events |= EPOLLRDHUP;
        }
        epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
        setnonblocking(fd);
    }



}

#endif