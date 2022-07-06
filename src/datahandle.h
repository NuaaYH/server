#pragma once
#include <functional>
#include <mysql/mysql.h>
#include <stdio.h>
#include <string>
using namespace std;
class Datahandle {

public: 
    Datahandle();
    ~Datahandle(){};
    bool logUp(const char* name,const char* pswd);
    bool logIn(const char* name,const char* pswd);
    int search_element(const char* name);
    void delete_element();
    void closemysql(){mysql_close(&conn);/*mysql_thread_end();*/};
private:    
    MYSQL conn;
};

Datahandle::Datahandle()
{
    //mysql_thread_init();
    mysql_init(&conn);
    
    //"root":数据库管理员 "123":root密码 "test":数据库的名字
    if(mysql_real_connect(&conn, "localhost","root","955330","test",0,NULL,CLIENT_FOUND_ROWS))
    {
        printf("connect success\n");
    }else
    {
        printf("connect failed\n");
    }
}

int Datahandle::search_element(const char* name)
{
    MYSQL_RES* result;
    char search_opt[255];
    sprintf(search_opt, "select * from test where name='%s'", name);
    int res = mysql_query(&conn, search_opt);
    result=mysql_store_result(&conn);
    if(mysql_fetch_row(result))
    {
        return 1;
    }else{
        return 0;
    }
}

bool Datahandle::logIn(const char* name,const char* pswd)
{
    MYSQL_RES* result;
    char search_opt[255];
    sprintf(search_opt, "select * from test where name='%s' and num='%s'", name, pswd);
    int res = mysql_query(&conn, search_opt);
    result=mysql_store_result(&conn);
    if(mysql_fetch_row(result))
    {
        return true;
    }else{
        return false;
    }
}

bool Datahandle::logUp(const char* name,const char* pswd)
{
    //printf("search_result:%d\n",search_element(name));
    if(search_element(name))
    {
        return false;
    }
    
    char insert_opt[255];
    sprintf(insert_opt, "insert into test values('%s','%s')", name,pswd);
    int res = mysql_query(&conn, insert_opt);
    if(res)
    {
        printf("error\n");
        return false;
    }
    else
    {
        printf("OK\n");
        return true;
    }
}
