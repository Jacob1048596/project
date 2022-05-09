#include<cstdio>
#include<iostream>
#include<sstream>
#include<mysql/mysql.h>
#include <string>
#include<list>
#include <json/json.h>

#include<mutex>
#define MYSQL_HOST "127.0.0.1"
#define MYSQL_USER "root"
#define MYSQL_PASS "114514"
#define MYSQL_DB "im_system"
using namespace std;

class db
{
private:
	/* data */
		MYSQL *_mysql;
		mutex _mutex;
		bool QuerySql(const string &sql)
		{
			if(mysql_query(_mysql,sql.c_str())!=0)
			{
				cout<<"query sql:"<<sql.c_str()<<"failed:"<<mysql_error(_mysql);
				return false;
			}
			return true;
		}
public:
	db(/* args */);
	~db();
};

db::db(/* args */):_mysql(NULL)
{
	_mysql=mysql_init(NULL);
	if(_mysql==NULL) 
	{
		cout<<"init mysql instance failed";
			exit(-1);
	}
	if(mysql_real_connect(_mysql,MYSQL_HOST,MYSQL_USER,MYSQL_PASS,MYSQL_DB,0,NULL,0)==NULL)
	{
		cout<<"connect mysql failed";
		mysql_close(_mysql);
		exit(-1);
	}
	if(mysql_set_character_set(_mysql,"utf8")!=0)
	{
		cout<<"set client character failed"<<mysql_error(_mysql);
		mysql_close(_mysql);
		exit(-1);
	}
}

db::~db()
{
	if(_mysql) mysql_close(_mysql);
}
