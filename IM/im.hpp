#include<cstdio>
#include<iostream>
#include<sstream>
#include<mysql/mysql.h>
#include <string>
#include<list>
#include <json/json.h>

#include<mutex>
#include"mongoose.h"
#include"db_user.hpp"
#include "db_message.hpp"
#define MYSQL_HOST "127.0.0.1"
#define MYSQL_USER "root"
#define MYSQL_PASS "114514"
#define MYSQL_DB "im_system"
#define ONLINE "onine"
#define OFFLINE "offline"
using namespace std;

namespace im{
	struct session{
		string name;
		uint64_t session_id;
		double login_time;
		double last_atime;
		mg_connection* conn;
	};

	class IM
	{
	public:
		IM();
		IM(IM &&) = default;
		IM(const IM &) = default;
		IM &operator=(IM &&) = default;
		IM &operator=(const IM &) = default;
		~IM();
		static bool Init(const string& port =":9000")
		{
			_tb_user=new TableUser();
			mg_mgr_init(&_mgr);
			string addr="0.0.0.0";
			addr+=port;
			_lis_http=mg_http_listen(&_mgr,addr.c_str(),callback,&_mgr);
			if(_lis_http==NULL)
			{
				cout<<"http listen failed\n";
				return false;
			}
			return true;
		}
		static bool Run()
		{
			while(1){mg_mgr_poll(&_mgr,1000);}
			return true;
		}
	private:
	static int Split(const string &str,const string &sep,vector<string> *list)
	{
		int count=0;
		size_t pos=0,idx=0;
		while(1)
		{
			pos=str.find(sep,idx);
			if(pos==string::npos)
			{
				break;
			}
			list->push_back(str.substr(idx,pos-idx));
			idx=pos+sep.size();
			count++;
		}
		if(idx<str.size())
		{
			list->push_back(str.substr(idx));
			count++;
		}
		return count;
	}
	//Cookie:SESSION_ID=1145141919810; NAME=zhangsan; path=/
	static bool GetCookie(const string &cookie,const string &key,string *val)
	{
		vector<string> list;
		int count=Split(cookie,"; ",&list);
		for(auto s:list)
		{
			vector<string> arry_cookie;
			Split(s,"=",&arry_cookie);
			if(arry_cookie[0]==key)
			{
				*val=arry_cookie[1];
				return true;
			}
		}
		return false;
	}
	static void CreatSession(session * s,mg_connection *c,const string &name)
	{
		s->name=name;
		s->session_id=(uint64_t)(mg_time()*1000000);
		s->login_time=mg_time();
		s->last_atime=mg_time();
		s->conn=c;
	}
	static void DeleteSession(mg_connection *c)
	{
		auto it=_list.begin();
		for(;it!=_list.end();it++)
		{
			if(it->conn==c)
			{
				cout<<"delete session:"<<it->name<<endl;
				_list.erase(it);
				return;
			}
		}
		return;
	}
	static session* GetSessionByConn(mg_connection *c)
	{
		auto it=_list.begin();
		for(;it!=_list.end();it++)
		{
			if(it->conn==c)
			{
				return &(*it);
			}
		}
		return NULL;

	}
	static session* GetSessionByName(const string &name)
	{
		auto it=_list.begin();
		for(;it!=_list.end();it++)
		{
			if(it->name==name)
			{
				return &(*it);
			}
		}
		return NULL;
	}

	static bool reg(mg_connection *c,mg_http_message* hm)
	{
		int status=200;
		string header="Content-Type:application/json\r\n";
		//从正文获取提交的用户信心（json字符串）
		string body;
		body.assign(hm->body.ptr,hm->body.len);
		//解析得到用户名和密码
		Json::Value user;
		Json::Reader reader;
		bool ret=reader.parse(body,user);
		if(ret==false)
		{
			status=400;
			mg_http_reply(c,status,header.c_str(),"{\"reason\":\"请求格式错误\"}");
			return false;
		}
		//判断用户名是否已存在
		ret=_tb_user->exists(user["name"].asString());
		if(ret==true)
		{
			status=400;
			mg_http_reply(c,status,header.c_str(),"{\"reason\":\"用户已存在\"}");
			return false;
		}
		//将用户信息插入数据库
		ret=_tb_user->Insert(user["name"].asString(),user["passwd"].asString());
		if(ret==false)
		{
			status=500;
			mg_http_reply(c,status,header.c_str(),"{\"reason\":\"数据库访问错误\"}");
			return false;
		}
		mg_http_reply(c,status,header.c_str(),"{\"reason\":\"注册成功\"}");
		return true;
	}
	static bool login(mg_connection *c,mg_http_message* hm)
	{
		int rsp_status=200;
		string rsp_body="{\"reason\":\"登录成功\"}";
		string rsp_header="Content-Type:application/json\r\n";
		string req_body;
		req_body.assign(hm->body.ptr,hm->body.len);
		Json::Value user;
		Json::Reader reader;
		bool ret=reader.parse(req_body,user);
		if (ret==false){
			rsp_status=400;
			rsp_body="{\"reason\":\"请求格式错误\"}";
			mg_http_reply(c,rsp_status,rsp_header.c_str(),rsp_body.c_str());
			return false;
		}
		ret=_tb_user->VerifyUser(user["name"].asString(),user["passwd"].asString());
			if (ret==false){
			rsp_status=403;
			rsp_body="{\"reason\":\"用户名或密码错误\"}";
			mg_http_reply(c,rsp_status,rsp_header.c_str(),rsp_body.c_str());
			return false;
		}
		ret=_tb_user->UpdateStatus(user["name"].asString(),ONLINE);
		if (ret==false){
			rsp_status=500;
			rsp_body="{\"reason\":\"修改用户状态出错\"}";
			mg_http_reply(c,rsp_status,rsp_header.c_str(),rsp_body.c_str());
			return false;
		}
		session s;
		CreatSession(&s,c,user["name"].asString());
		_list.push_back(s);
		stringstream cookie;
		cookie<<"Set-Cookie:SESSION_ID="<<s.session_id<<"; NAME="<<s.name<<"; pash=/\r\n";
		cookie<<"Set-Cookie:NAME="<<s.name<<"; pash=/\r\n";
		rsp_header+=cookie.str();

		mg_http_reply(c,rsp_status,rsp_header.c_str(),rsp_body.c_str());
		return true;
	}
	static void Broadcast(const string &msg)
	{
		mg_connection *c;
		for(c=_mgr.conns;c!=NULL;c=c->next)
		{
			if(c->is_websocket){
				mg_ws_send(c,msg.c_str(),msg.size(),WEBSOCKET_OP_TEXT);
			}
		}
		return ;
	}
	static void SendByName(const string name,const string &msg){
		mg_connection *c;

		auto it=_list.begin();
		for(;it!=_list.end();it++)
		{
			if(it->name==name)
			{
				c=it->conn;
				if(c->is_websocket){
				mg_ws_send(c,msg.c_str(),msg.size(),WEBSOCKET_OP_TEXT);
				}
			}
		}
		return ;
	}
	
	static void showOnineUser(mg_connection *c,mg_http_message* hm){
		Json::Value users;
		int rsp_status=200;
		bool ret=_tb_user->SelectOnlineUser(&users);
		 if(ret==false)
   		 {
        		printf("GetAll from database filed!\n");
       		 	rsp_status=500;
        		return ;
  		 }
		  Json::FastWriter writer;
		  string header="Content-Type:application/json\r\n";
		  mg_http_reply(c,rsp_status,header.c_str(),writer.write(users).c_str());
    		
	}

	static void showAllUser(mg_connection *c,mg_http_message* hm){
		Json::Value users;
		int rsp_status=200;
		bool ret=_tb_user->SelectAllInfo(&users);
		 if(ret==false)
   		 {
        		printf("GetAll from database filed!\n");
       		 	rsp_status=500;
        		return ;
  		 }
		  Json::FastWriter writer;
		  string header="Content-Type:application/json\r\n";
		  mg_http_reply(c,rsp_status,header.c_str(),writer.write(users).c_str());
    		
	}
	static void Message(string msg)
	{
		
		//Broadcast(msg);
		
		Json::Value Message;
		Json::Reader reader;
		bool ret=reader.parse(msg,Message);
		if (ret==false)
		{
			printf("json error");
			return ;
		} 
		
		if(Message["send_name"]=="global")
		{
			
			//string public_msg=Message.toStyledString() ;
			string public_msg="<p>"+Message["user_name"].asString()+":"+Message["send_msg"].asString()+" time :"+Message["msg_time"].asString()+"<p>";
			Broadcast(public_msg);
		}
		else{
			//SendByName(Message["send_name"].asString(),Message["send_msg"].asString());
		}
			
	}
	static void callback(mg_connection *c, int ev, void *ev_data, void *fn_data)
	{
		struct mg_http_message* hm=(struct mg_http_message*)ev_data;
		struct mg_ws_message* wm=(mg_ws_message*)ev_data;
		
		switch (ev)
		{
		case MG_EV_HTTP_MSG:
			if(mg_http_match_uri(hm,"/reg"))
			{	//注册的提交表单的数据请求
				reg(c,hm);

			}else if(mg_http_match_uri(hm,"/login"))
			{	//登录的提交表单的数据请求
				login(c,hm);
			}
			else if(mg_http_match_uri(hm,"/ShowOnline")){
				showOnineUser(c,hm);

			}else if(mg_http_match_uri(hm,"/ShowAllUser")){
				showAllUser(c,hm);

			}else if(mg_http_match_uri(hm,"/websocket"))
			{	//websocket的升级请求
				//建立ws聊天的时候，应检测登陆状态

				struct mg_str*cookie_str=mg_http_get_header(hm,"Cookie");
				if(cookie_str==NULL)
				{
					//未登录用户
					string body=R"({"reason":"未登录"})";
					string header="Content-Type:application/json\r\n";
					mg_http_reply(c,403,header.c_str(),body.c_str());
					return;
				}
				string name;
				string tmp;
				tmp.assign(cookie_str->ptr,cookie_str->len);
				GetCookie(tmp,"NAME",&name);
				string msg=name+"加入聊天室";
				Broadcast(msg);
				mg_ws_upgrade(c,hm,NULL);//确认升级websocket

			}else{
				//除了登陆页面，都应该检测cookie，判断是否登录
				//如果没有session，则应该跳转到登录页面
				if(hm->uri.ptr!="/login.html")
				{
					//获取cookie，根据name找session，若没有就是没登陆
					//
				}
				mg_http_serve_opts opts={.root_dir="./web_root"};
				mg_http_serve_dir(c,hm,&opts);//静态页面资源响应
			}
			break;
		case MG_EV_WS_MSG:
		{
			// if(cookie_str==NULL)
			// 	{
			// 		//未登录用户
					
			// 		string body=R"({"reason":"未登录"})";
			// 		string header="Content-Type:application/json\r\n";
			// 		mg_http_reply(c,403,header.c_str(),body.c_str());
			// 		return;
			// 	}
			string msg;
			msg.assign(wm->data.ptr,wm->data.len);
			Message(msg);
		}
			break;
		case MG_EV_CLOSE:
		{
			struct session *ss =GetSessionByConn(c);
			if(ss!=NULL)
			{

				string msg=ss->name+"离开聊天室";
				Broadcast(msg);
				_tb_user->UpdateStatus(ss->name,OFFLINE);
				DeleteSession(c);
			}
		}
			break;
		default:
			break;
		}
	}
		static TableUser *_tb_user;
		static mg_mgr _mgr;//句柄
		static mg_connection *_lis_http;//监听连接
		string _addr;//地址信息
		static list<session> _list;
	};

	IM::IM()
	{
}
	IM::~IM()
	{
		mg_mgr_free(&_mgr);
	}
	TableUser* IM::_tb_user=NULL;
	mg_mgr IM:: _mgr;
	mg_connection* IM::_lis_http=NULL;
	list<session> IM::_list;
}
