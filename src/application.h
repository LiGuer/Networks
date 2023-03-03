#ifndef APPLICATION_H
#define APPLICATION_H

#include "include.h"
#include "HTTP.h"
#include "logIn_register.h"

namespace Networks {

void app_http(string rev_str, int connection_socket_fd, ClientData* user, int epoll_fd, MYSQL& m_mysql) {

    // struct 
    HTTP::HttpRequset  hq;
    HTTP::HttpResponse hs;

    read(hq, rev_str);

    if (hq.urlFields.find("action") != hq.urlFields.end()) {
        
        if (hq.urlFields["action"] == "login") {
            int id = atoi(hq.urlFields["user_id"].c_str());
            long long key = atoi(hq.urlFields["password"].c_str());
            string name;
            
            if(!check_user_key(m_mysql, id, key, name)) {
                hs.headerFields["content_type"] = "text/plain";
                hs.body = "Wrong ID or password!";

                user->buf = to_string(hs);
                Networks::send_message_epoll(connection_socket_fd, epoll_fd);
                return;
            }
        }
    }

    hq.url = HTTP::url_process(hq.url);
    hs.headerFields["content_type"] = HTTP::file_type(hq.url);
    readFile(hq.url, hs.body);

    user->buf = to_string(hs);
    Networks::send_message_epoll(connection_socket_fd, epoll_fd);
}
    
}

#endif