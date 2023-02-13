#ifndef MYSQL_H
#define MYSQL_H
#include <mysql/mysql.h>

namespace Networks {
    MYSQL mysql_init() {
        MYSQL com_mysql;

        if (mysql_init(&com_mysql) == NULL) {
            printf("Cannot init mysql!\n");
            exit(-1);
        }

        MYSQL* ret = mysql_real_connect(
            &com_mysql,
            MYSQL_IP,
            "debian-sys-maint",
            "kUpaezk5soMAwGA0",
            "USERS",
            0, NULL, 0
        );

        if (ret == NULL) {
            printf("%s\n", mysql_error(&com_mysql));
            exit(-1);
        }
        else
            printf("Mysql Access Successful.\n");

        return com_mysql;
    }

    bool check_user_key(MYSQL& com_mysql, int id, std::string& key, std::string& name) {
        int n = key.size();
        long long key_hash = 0;
        /*
        for(int i = 0; i < n; i++) {
            if(key[i] <= '0' && key[i] >= '9')
                key_hash += (key[i] - '0');

            else if(key[i] <= 'a' && key[i] >= 'z') 
                key_hash += (key[i] - 'a') + 10;

            else if(key[i] <= 'A' && key[i] >= 'Z') 
                key_hash += (key[i] - 'A') + 36;

            else 
                key_hash += key[i];

            key_hash *= 10;
        }*/
        key_hash = atoi(key.c_str());

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
            if(atoi(hs[2]) == key_hash) {
                name = hs[1];
                mysql_free_result(pRes);
                return true;
            }
        }
        mysql_free_result(pRes);
        return false;
    }

    bool insert_new_user(MYSQL& com_mysql, int id, std::string& key) {
        int n = key.size();
        long long key_hash = 0;

        for(int i = 0; i < n; i++) {
            if(key[i] <= '0' && key[i] >= '9')
                key_hash += (key[i] - '0');

            else if(key[i] <= 'a' && key[i] >= 'z') 
                key_hash += (key[i] - 'a') + 10;

            else if(key[i] <= 'A' && key[i] >= 'Z') 
                key_hash += (key[i] - 'A') + 36;

            else 
                key_hash += key[i];

            key_hash *= 10;
        }

        return true;
    }
}
#endif