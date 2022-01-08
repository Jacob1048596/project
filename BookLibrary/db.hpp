#include<iostream>
#include<mysql/mysql.h>
#include<json/json.h>
#include<mutex>

#define MYSQL_HOST "127.0.0.1"
#define MYSQL_USER "root"
#define MYSQL_PSWD "114514"
#define MYSQL_DB "db_Library"

 static std::mutex g_mutex;
MYSQL*  MysqlInit()
{
	MYSQL *mysql;
         //初始化
         mysql=mysql_init(NULL);
        if(mysql==NULL)
         {
         printf("init mysql error\n");
             return NULL;
        }
         //连接服务器
        if(mysql_real_connect(mysql,MYSQL_HOST,MYSQL_USER,MYSQL_PSWD,NULL,0,NULL,0)==NULL)
        {
            printf("connect mysql error :%s\n",mysql_error(mysql));
            mysql_close(mysql);
            return NULL;
        }
        //选择字符集
        if(mysql_set_character_set(mysql,"utf8")!=0)
        {
            printf("connect mysql error :%s\n",mysql_error(mysql));
            mysql_close(mysql);
            return NULL;
        }
        //选择数据库
        if(mysql_select_db(mysql,MYSQL_DB)!=0)
        {
            printf("select db error :%s\n",mysql_error(mysql));
            return NULL;
        }
        return mysql;
}

  void MysqlRelease(MYSQL *mysql)//销毁句柄
    {
        if(mysql)
        {
            mysql_close(mysql);
        }
        return ;
    }
 bool MysqlQuery(MYSQL *mysql,const char* sql)//执行语句的共有接口
    {
        int ret=mysql_query(mysql,sql);
        if(ret!=0)
        {
            printf("squery fild [%s] error faid:%s\n",sql,mysql_error(mysql));
            return false;
        }
        return true;
    }