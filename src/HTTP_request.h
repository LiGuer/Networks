#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <iostream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

void string_split(string str, const string& splits, vector<string>& answer) {
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

namespace HTTP {
/*
 *  const value
 */

/*
 *  baisc struct
 */

// Http Requset Struct
class HttpRequset {
public:
    // variable
    string method;
    string url;
    string version = "HTTP/1.1";
    vector<pair<string, string>> headerFields;
    string body;

    /*
     *   HttpRequset to string
     */
    string to_string() {
        // RFC 7230, 3.1.1. Request Line
        string requestLine = method + " " + url + " " + version + "\r\n";

        // RFC 7230, 3.2. Header Fields
        string headerFieldsResult;

        for (auto& e : headerFields) {
            headerFieldsResult += e.first + ": " + e.second + "\r\n";
        }

        // result
        string result = requestLine + headerFieldsResult + "\r\n";

        return result;
    }

    /*
     *   string to HttpRequset
     */
    HttpRequset& read(string& request) {
        vector<string> requestList;

        string_split(request, "\n", requestList);

        {
            vector<string> t;
            string_split(requestList[0], " ", t);

            method = t[0];
            url = t[1];
            version = t[2];
        }

        for (int i = 1; i < requestList.size(); i++) {
            vector<string> t;
            string_split(requestList[i], ": ", t);
            headerFields.push_back({ t[0], t[1] });
        }

        return *this;
    }
};

// Http Response Struct
class HttpResponse {
    string version = "HTTP/1.1";
    int status = 200;
    vector<pair<string, string>> headerFields;
    string body;

    /*
     *   HttpResponse to string
     */
    string to_string() {
        string responseLine = version + " " + std::to_string(status) + " " + "" + "\r\n";

        string headerFieldsResult;

        for (auto& e : headerFields) {
            headerFieldsResult += e.first + ": " + e.second + "\r\n";
        }

        // result
        string result = responseLine + headerFieldsResult + "\r\n";

        return result;
    }

    /*
     *   string to HttpRequset
     */
    HttpResponse& read(string& request) {
        vector<string> requestList;

        string_split(request, "\n", requestList);

        {
            vector<string> t;
            string_split(requestList[0], " ", t);

            version = t[0];
            status = atoi(t[1].c_str());
        }

        for (int i = 1; i < requestList.size(); i++) {
            vector<string> t;
            string_split(requestList[i], ": ", t);
            headerFields.push_back({ t[0], t[1] });
        }

        return *this;
    }
};


}
#endif