#ifndef LOGIN_REGISTER_H
#define LOGIN_REGISTER_H

#include "include.h"
#include "basic.h"
#include "Mysql.h"

namespace Networks {
    /*
     *  check uer's id and key
     */
    bool check_user_key(MYSQL& com_mysql, int id, long long& key, std::string& name) {
        std::string sql_command;
        sql_command = "select * from users where id = " + std::to_string(id) + ';';

        if (mysql_query(&com_mysql, sql_command.c_str()) != 0) {
            printf("%s\n", mysql_error(&com_mysql));
            return false;
        }

        MYSQL_RES* pRes;
        MYSQL_ROW hs;

        if ((pRes = mysql_store_result(&com_mysql)) == NULL) {
            printf("%s\n", mysql_error(&com_mysql));
            return 0;
        }
        while (hs = mysql_fetch_row(pRes)) {
            if (atoi(hs[2]) == key) {
                name = hs[1];
                mysql_free_result(pRes);
                return true;
            }
        }
        mysql_free_result(pRes);
        return false;
    }

    /*
    *  insert a new user and return its id
    */
    bool insert_new_user(MYSQL& com_mysql, int& id, std::string& name, long long& key) {
        std::string sql_command;

        // query the number of users
        sql_command = "select * from users;";

        if (mysql_query(&com_mysql, sql_command.c_str()) != 0) {
            printf("%s\n", mysql_error(&com_mysql));
            return false;
        }

        MYSQL_RES* pRes;

        if ((pRes = mysql_store_result(&com_mysql)) == NULL) {
            printf("%s\n", mysql_error(&com_mysql));
            return false;
        }
        id = mysql_num_rows(pRes) + 1;

        // insert new data
        sql_command = "insert into users values ( id = " + std::to_string(id) + ", name = `" + name + "`, key = " + std::to_string(key) + ");";

        if (mysql_query(&com_mysql, sql_command.c_str()) != 0) {
            printf("%s\n", mysql_error(&com_mysql));
            return false;
        }

        return true;
    }

    /*
     *  rename a user
     */
    bool rename_user(MYSQL& com_mysql, int id, std::string& name, long long& key) {
        std::string sql_command;
        sql_command = "update users set name = `" + name + "` where id = " + std::to_string(id) + ");";

        if (mysql_query(&com_mysql, sql_command.c_str()) != 0) {
            printf("%s\n", mysql_error(&com_mysql));
            return false;
        }
        return true;
    }


    void log_in(MYSQL& com_mysql, int connection_socket_fd, int epollfd, Networks::ClientData& user) {

        string 
            id_key = user.buf, 
            name, 
            message;
        vector<string> id_key_vector;
        int id = 0;
        long long key;

        string_split(id_key, " ", id_key_vector);

        for (int i = 0; i < id_key_vector.size(); i++) {
            if (id == 0 &&
                id_key_vector[i].size() != 0) {
                id = atoi(id_key_vector[i].c_str());
            }
            else if (id_key_vector[i].size() != 0) {
                key = atoi(id_key_vector[i].c_str());
                break;
            }
        }

        if (Networks::check_user_key(com_mysql, id, key, name)) {
            user.id = id;
            message = "Welcome back, " + name + "!\n";
        }
        else {
            message = "Wrong ID or KEY!\n";
        }
        user.buf = message;
        Networks::send_message_epoll(connection_socket_fd, epollfd);
    }

}
#endif