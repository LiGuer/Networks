#ifndef APPLICATION_H
#define APPLICATION_H

#include "include.h"
#include "HTTP.h"
#include "logIn_register.h"

namespace Networks {

void app_(string rev_str, int connection_socket_fd, ClientData* user, int epoll_fd) {
    user->buf = HTTP::http(rev_str);
    Networks::send_message_epoll(connection_socket_fd, epoll_fd);
}

void app_0(int connection_socket_fd, int users_fd, ClientData& user, int epoll_fd, MYSQL& m_mysql) {
    // get the id and key of clients
    if (user.id == 0) {
        Networks::log_in(
            m_mysql,
            connection_socket_fd,
            epoll_fd, user
        );
    }
}
    
}

#endif