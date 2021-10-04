#include <iostream>
#include<string>
#include "../mongoose.h"

void callback(struct mg_connection *c, int ev, void *ev_data, void *fn_data)
{
	switch(ev)
	{
		case MG_EV_HTTP_MSG:
		{
			struct mg_http_message* hm=(struct mg_http_message*)ev_data;
			std::cout<<hm->method.ptr<<std::endl;
			std::cout<<hm->uri.ptr<<std::endl;
			if(mg_http_match_uri(hm,"/hello"))
			{
				//if(hm->method.ptr=="GET")
				//{
					std::string body="<html><body><h1>Hello Bit</h1></body></html>";
					std::string header="Content-Type:text/html\r\n";
					mg_http_reply(c,200,header.c_str(),"%s",body.c_str());//自主组织响应
				//}
			}
			else if(mg_http_match_uri(hm,"/websocket"))
			{
				//当前链接一旦升级ws 下次有数据就是触发ws——msg事件
				mg_ws_upgrade(c,hm,NULL);//确认升级websocket

mg_http_serve_opts opts={.root_dir="./web_root"};
					mg_http_serve_dir(c,hm,&opts);//静态页面资源响应			}else{
					
			}
			
		}break;
		case MG_EV_WS_MSG:
		{
			struct mg_ws_message* wm=(mg_ws_message*)ev_data;
			break;
		}
		case MG_EV_CLOSE:break;
		default:break;
	}
	return;
}
int main()
{
	//定义句柄
	struct mg_mgr mgr;
	//初始化句柄
	mg_mgr_init(&mgr);
	//创建连接
	struct mg_connection *lst_hpp;
	lst_hpp=mg_http_listen(&mgr,"0.0.0.0:9000",callback,NULL);
	if(lst_hpp==NULL)
		return -1;
	//开始监听
	while(1)
	{
		mg_mgr_poll(&mgr,1000);
	}
	
	return 0;
}