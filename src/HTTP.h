#ifndef HTTP_H
#define HTTP_H

#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include "HTTP_message.h"

using namespace std;

namespace HTTP {

inline string url_process(string url) {
    if (url == "/")
        url = "../html/index.html";
    else 
        url = "../html" + url;
    return url;
}

inline string file_type(string url) {
    string ret = "";
    int st = url.size() - 1;

    while(url[st] != '.')
        st--;

    for (int i = st + 1; i < url.size(); i++) 
        ret += url[i];

    if(ret == "js") 
        ret = "text/javascript";
    else if (ret == "css")
        ret = "text/css"; 
    else if (ret == "html")
        ret = "text/html"; 
    else if (ret == "jpg" || ret == "png") 
        ret = "text/img";
    else
        ret = "text/plain"; 

    return ret;
}

string bad_request(int error_code) {
    string ret = "";

    switch (error_code) {
    case 400:
        ret = string("HTTP/1.0 400 BAD REQUEST\r\n") +
            "Content-type: text/html\r\n" +
            "\r\n" +
            "<P>Your browser sent a bad request, " +
            "such as a POST without a Content-Length.\r\n";
        break;
    case 500:
        ret = string("HTTP/1.0 500 Internal Server Error\r\n") +
            "Content-type: text/html\r\n" +
            "\r\n" +
            "<P>Error prohibited CGI execution.\r\n";
        break;
    }
    return ret;
}

string execute_cgi(int client, string path, int method, string query_string) {
    int cgi_in[2], 
        cgi_out[2];

    if (pipe(cgi_out) < 0) {
        return bad_request(500);
    }
    if (pipe(cgi_in) < 0) {
        return bad_request(500);
    }

    pid_t pid;
    if ((pid = fork()) < 0) {
        return bad_request(500);
    }

    string ret = "HTTP/1.0 200 OK\r\n";

    if (pid == 0) { /* child: CGI script
        dup2(cgi_out[1], STDOUT);
        dup2(cgi_in[0], STDIN);
        close(cgi_out[0]);
        close(cgi_in[1]);

        if (method == GET) {
            sprintf(query_env, "QUERY_STRING=%s", query_string);
            putenv(query_env);
        }
        else if (method == POST) {
            sprintf(length_env, "CONTENT_LENGTH=%d", content_length);
            putenv(length_env);
        }
        execl(path, NULL); */
        exit(0);

    } else {    /* parent 
        close(cgi_out[1]);
        close(cgi_in[0]);

        if (strcasecmp(method, "POST") == 0)
            for (i = 0; i < content_length; i++) {
                recv(client, &c, 1, 0);
                write(cgi_in[1], &c, 1);
            }
        while (read(cgi_out[0], &c, 1) > 0)
            send(client, &c, 1, 0);

        close(cgi_out[0]);
        close(cgi_in[1]);
        waitpid(pid, &status, 0);*/
    }
    return ret;
}

inline string http(string in) { 
    // struct 
    HttpRequset  hq;
    HttpResponse hs;

    read(hq, in);
    string url = url_process(hq.url);
    hs.content_type = file_type(url);

    // read files
    FILE *fin = fopen(url.c_str(), "r");
    char ch;
    while ((ch = fgetc(fin)) != EOF) 
        hs.body += ch;
    fclose(fin);

    return to_string(hs);
}

}
#endif