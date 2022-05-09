#include"db.hpp"

class TableMessage
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

		TableMessage(/* args */);
		~TableMessage();
		bool Insert(Json::Value Message){
		#define INSERT_MSG "insert tb_message value(null,'%s','%s','%s','%s','%s');"
		char tmp_sql[4096]={0};
		sprintf(tmp_sql,INSERT_MSG, Message["send_name"].asString().c_str(),
									Message["send_message"].asString().c_str(),
									Message["user_name"].asString().c_str(),
									Message["msg_type"].asString().c_str(),
									Message["msg_time"].asString().c_str());
		return QuerySql(tmp_sql);
		}
		bool Delete(int Mid){
			#define DELETE_MSG "delete from tb_message where id='%d';"
			char tmp_sql[4096]={0};
			sprintf(tmp_sql,DELETE_MSG,Mid);
			return QuerySql(tmp_sql);
			return true;
		}
		bool update(int Mid){

		}
		bool SearchBySendName(const string &name,Json::Value* Message){

		}
	};
	
	TableMessage::TableMessage(/* args */)
	{
		
	}
	
	TableMessage::~TableMessage()
	{

	}