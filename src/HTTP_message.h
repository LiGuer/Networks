#ifndef HTTP_MESSAGE_H
#define HTTP_MESSAGE_H

#include <string>
#include <utility>
#include <vector>
#include "include.h"

namespace HTTP {
enum {GET = 1, POST};

/*
 *  basic struct
 */

struct HttpRequset {
    string 
        method = "",
        url = "",
        version = "HTTP/1.1",
        body = "";

    map<string, string> headerFields;
    map<string, string> urlFields;
};

struct HttpResponse {
    int status = 200;
    
    string 
        version = "HTTP/1.1",
        body = "";

    map<string, string> headerFields;
};

/*
 *  analysis message
 */
HttpRequset& read(HttpRequset& hp, string& in) {
    vector<string> requestList;

    string_split(in, "\r\n", requestList);

    int i = 0;
    {
        vector<string> t;
        string_split(requestList[i], " ", t);

        hp.method = t[0];
        hp.url = t[1];
        hp.version = t[2];
    }

    // url
    {
        int pos = hp.url.find('?');

        if (pos != hp.url.npos) {

            string addition = hp.url.substr(pos + 1), t = "";
            pair<string, string> pt;
            
            for (auto& c : addition) {
                if (c == '&' || c == '=') {
                    if (c == '&') {
                        pt.second = t;
                        hp.urlFields[pt.first] = pt.second;
                    } 
                    else if(c == '=') 
                        pt.first = t;

                    t = "";
                } else 
                    t += c;
            }

            pt.second = t;
            hp.urlFields[pt.first] = pt.second;

            hp.url = hp.url.substr(0, pos);
        }
    }

    for (i = 1; i < requestList.size(); i++) {
        if (requestList[i].size() == 0)
            break;

        vector<string> t;
        string_split(requestList[i], ": ", t);
        hp.headerFields[t[0]] = t[1];
    }

    // body
    i++;
    for (; i < requestList.size(); i++) {
        hp.body += requestList[i];
    }
    return hp;
}

HttpResponse& read(HttpResponse& hp, string& in) {
        vector<string> requestList;

        string_split(in, "\r\n", requestList);

        int i = 0;

        {
            vector<string> t;
            string_split(requestList[i], " ", t);

            hp.version = t[0];
            hp.status = atoi(t[1].c_str());
        }

        for (i = 1; i < requestList.size(); i++) {
            if (requestList[i].size() == 0)
                break;

            vector<string> t;
            string_split(requestList[i], ": ", t);
            hp.headerFields[t[0]] = t[1];
        }

        // body
        i++;
        for (; i < requestList.size(); i++) {
            hp.body += requestList[i];
        }

        return hp;
}

/*
 *  to string
 */
string to_string(HttpRequset& hp) {
    // RFC 7230, 3.1.1. Request Line
    string requestLine = hp.method + " " + hp.url + " " + hp.version + "\r\n";

    // RFC 7230, 3.2. Header Fields
    string headerFieldsResult;
    for (auto& e : hp.headerFields) {
        headerFieldsResult += e.first + ": " + e.second + "\r\n";
    }

    // result
    string ret = requestLine + headerFieldsResult + "\r\n";

    return ret;
}

string to_string(HttpResponse& hp) {
    string responseLine = hp.version + " " + std::to_string(hp.status) + " " + "" + "\r\n";

    hp.headerFields["Content-length"] = std::to_string(hp.body.size());

    string headerFieldsResult;
    for (auto& e : hp.headerFields) {
        headerFieldsResult += e.first + ": " + e.second + "\r\n";
    }

    // result
    string ret = responseLine + headerFieldsResult + "\r\n" + hp.body+ "\r\n";

    return ret;
}

}
#endif