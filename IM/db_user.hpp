#include<cstdio>
#include<iostream>
#include<sstream>
#include<mysql/mysql.h>
#include <string>
#include<list>
#include <json/json.h>

#include<mutex>
#include"mongoose.h"
#define MYSQL_HOST "127.0.0.1"
#define MYSQL_USER "root"
#define MYSQL_PASS "114514"
#define MYSQL_DB "im_system"
#define ONLINE "onine"
#define OFFLINE "offline"
using namespace std;
	class TableUser
	{
	public:
		TableUser();
		TableUser(TableUser &&) = default;
		TableUser(const TableUser &) = default;
		TableUser &operator=(TableUser &&) = default;
		TableUser &operator=(const TableUser &) = default;
		~TableUser();
	bool Insert(const string &name,const string &passwd){
		#define INSERT_USER "insert tb_user value(null,'%s',MD5('%s'),'%s');"
		char tmp_sql[4096]={0};
		sprintf(tmp_sql,INSERT_USER,name.c_str(),passwd.c_str(),OFFLINE);
		return QuerySql(tmp_sql);
		//return true;
	}
	bool Delete(const string &name){
		#define DELETE_USER "delete from tb_user where name='%s';"
		char tmp_sql[4096]={0};
		sprintf(tmp_sql,DELETE_USER,name.c_str());
		return QuerySql(tmp_sql);
		return true;
	}
	bool UpdateStatus(const string &name,const string &status){
		#define UPDATE_USER_STATUS "update tb_user set status='%s' where name='%s';"
		char tmp_sql[4096]={0};
		sprintf(tmp_sql,UPDATE_USER_STATUS,status.c_str(), name.c_str());
		return QuerySql(tmp_sql);
		
	}
	bool UpdatePasswd(const string &name,const string &passwd){
		#define UPDATE_USER_PASSWD "update tb_user set passwd=MD5('%s') where name='%s';"
		char tmp_sql[4096]={0};
		sprintf(tmp_sql,UPDATE_USER_PASSWD,passwd.c_str(), name.c_str());
		return QuerySql(tmp_sql);
		
	}
	bool SelectOne(const string &name,Json::Value *user){
		#define SELECT_USER_ONE "select id,passwd,status from tb_user where name='%s';"
		char tmp_sql[4096]={0};
		sprintf(tmp_sql,SELECT_USER_ONE,name.c_str());
		_mutex.lock();
		if(QuerySql(tmp_sql)==false)
		{
			_mutex.unlock();
			return false;
		}
			
		MYSQL_RES *res=mysql_store_result(_mysql);
		_mutex.unlock();
		if(res==NULL)
		{
			cout<<"select one user store result failed"<<mysql_error(_mysql)<<endl;
			return false;
		}
		int num_row=mysql_num_rows(res);
		if(num_row!=1)
		{
			cout<<"one user result number count failed";
			mysql_free_result(res);
			return false;
		}
		for(int i=0;i<num_row;i++)
		{
			MYSQL_ROW row=mysql_fetch_row(res);
			(*user)["id"]=stoi(row[0]);
			(*user)["name"]=name.c_str();
			(*user)["passwd"]=row[1];
			(*user)["status"]=row[2];
		}
		mysql_free_result(res);
		return true;
	}
	bool SelectAll(Json::Value *users){
		#define SELECT_USER_ALL "select id,name,passwd,status from tb_user ;"
		char tmp_sql[4096]={0};
		sprintf(tmp_sql,SELECT_USER_ALL);
		_mutex.lock();
		if(QuerySql(tmp_sql)==false)
		{
			_mutex.unlock();
			return false;
		}
			
		MYSQL_RES *res=mysql_store_result(_mysql);
		_mutex.unlock();
		if(res==NULL)
		{
			cout<<"select all user store result failed"<<mysql_error(_mysql)<<endl;
			return false;
		}
		int num_row=mysql_num_rows(res);
		for(int i=0;i<num_row;i++)
		{
			Json::Value user;
			MYSQL_ROW row=mysql_fetch_row(res);
			user["id"]=stoi(row[0]);
			user["name"]=row[1];
			user["passwd"]=row[2];
			user["status"]=row[3];
			users->append(user);
		}
		mysql_free_result(res);
		return true;
	}
	bool SelectAllInfo(Json::Value *users){
		#define SELECT_USER_ALL "select id,name,status from tb_user ;"
		char tmp_sql[4096]={0};
		sprintf(tmp_sql,SELECT_USER_ALL);
		_mutex.lock();
		if(QuerySql(tmp_sql)==false)
		{
			_mutex.unlock();
			return false;
		}
			
		MYSQL_RES *res=mysql_store_result(_mysql);
		_mutex.unlock();
		if(res==NULL)
		{
			cout<<"select all user info store result failed"<<mysql_error(_mysql)<<endl;
			return false;
		}
		int num_row=mysql_num_rows(res);
		for(int i=0;i<num_row;i++)
		{
			Json::Value user;
			MYSQL_ROW row=mysql_fetch_row(res);
			user["id"]=stoi(row[0]);
			user["name"]=row[1];
			user["status"]=row[2];
			users->append(user);
		}
		mysql_free_result(res);
		return true;
	}
	bool SelectOnlineUser(Json::Value *users){
		#define SELECT_USER_ONLINE "select name from tb_user where status='%s';"
		char tmp_sql[4096]={0};
		sprintf(tmp_sql, SELECT_USER_ONLINE,ONLINE);
		_mutex.lock();
		if(QuerySql(tmp_sql)==false)
		{
			_mutex.unlock();
			return false;
		}
			
		MYSQL_RES *res=mysql_store_result(_mysql);
		_mutex.unlock();
		if(res==NULL)
		{
			cout<<"select all user store result failed"<<mysql_error(_mysql)<<endl;
			return false;
		}
		int num_row=mysql_num_rows(res);
		for(int i=0;i<num_row;i++)
		{
			Json::Value user;
			MYSQL_ROW row=mysql_fetch_row(res);
			user["name"]=row[0];
			users->append(user);
		}
		mysql_free_result(res);
		return true;

	}
	bool VerifyUser(const string &name,const string &passwd){
		
		#define VERIFY_USER "select * from tb_user where name='%s'and passwd=MD5('%s');"
		char tmp_sql[4096]={0};
		sprintf(tmp_sql,VERIFY_USER,name.c_str(),passwd.c_str());	
		_mutex.lock();	
		if(QuerySql(tmp_sql)==false)
		{
			_mutex.unlock();
			return false;
		}
		
		MYSQL_RES *res=mysql_store_result(_mysql);
		_mutex.unlock();
		if(res==NULL)
		{
			cout<<"verify user store result failed"<<mysql_error(_mysql)<<endl;
			return false;
		}
		int num_row=mysql_num_rows(res);
		if(num_row!=1)
		{
			cout<<"verify user store result failed"<<endl;
			mysql_free_result(res);
			return false;
		}
		mysql_free_result(res);
		return true;
	}
	bool exists(const string &name){
		
		#define EXISTS_USER "select * from tb_user where name='%s';"
		char tmp_sql[4096]={0};
		sprintf(tmp_sql,EXISTS_USER,name.c_str());
		_mutex.lock();
		if(QuerySql(tmp_sql)==false)
		{
			_mutex.unlock();
			return false;
		}
			
		MYSQL_RES *res=mysql_store_result(_mysql);
		_mutex.unlock();
		if(res==NULL)
		{
			cout<<"exists user store result failed"<<mysql_error(_mysql)<<endl;
			return false;
		}
		int num_row=mysql_num_rows(res);
		if(num_row!=1)
		{
			cout<<"have no user"<<endl;
			mysql_free_result(res);
			return false;
		}
		mysql_free_result(res);
		return true;
	}

	private:
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
	};
	
	TableUser::TableUser():_mysql(NULL)
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
		//mysql_select_db(_mysql,MYSQL_DB);

	}
	
	TableUser::~TableUser()
	{
		if(_mysql) mysql_close(_mysql);
	}