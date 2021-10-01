#include<stdio.h>
#include<stdlib.h>
#include<mysql/mysql.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>
int main()
{
    MYSQL *mysql=mysql_init(NULL);
    if(mysql==NULL)
    {
        perror("mysql init error"),exit(0);
    }
    if(mysql_real_connect(mysql,"172.17.213.69",'root','114514','db_blog',0,NULL,0)==NULL)
    {
        printf("connect filed:%s\n",mysql_error(mysql));
        return -1;
    }
    if(mysql_set_character_set(mysql,'utf8')!=0)
    {
        printf("set character filed:%s\n",mysql_error(mysql));
        return -1;
    }
    if(mysql_select_db(mysql,'db_blog')!=0)
    {
          printf("select db filed:%s\n",mysql_error(mysql));
        return -1;
    }
    
    char *sql_str=" create table if not exit tb_stu( id int,name varchar(32),info text,  score decimal(4,2),    birth datetime);";
       return 0;
}



