#include<cstdio>
#include<iostream>
#include<mysql/mysql.h>
#include <string>
#include <json/json.h>
using namespace std;

namespace im{
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
		return true;
	}
	bool Delete(const string &name){
		return true;
	}
	bool UpdateStatus(const string &name,const string &status){
		return true;
	}
	bool UpdatePasswd(const string &name,const string &status){
		return true;
	}
	bool SelectOne(const string &name,Json::Value *user)
	{
		return true;
	}
	bool SelectAll(Json::Value *users)
	{
		return true;
	}
	bool VerifyUser(const string &name,const string &passwd)
	{
		return true;
	}
	bool exists(const string &name)
	{
		return true;
	}
	private:
		MYSQL *_mysql;
	};
	
	TableUser::TableUser()
	{
	}
	
	TableUser::~TableUser()
	{
	}
}
