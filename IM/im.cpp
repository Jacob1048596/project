#include "im.hpp"
void sql_test()
{
	// im::TableUser user;
	//user.Insert("zhangsa","123456");
	//user.UpdatePasswd("zhangsan","111111");
	//cout<<"1"<<endl;
	//cout<<user.exists("lis")<<endl;
	// Json::Value User;
	// user.SelectAll(&User);
	// Json::StyledWriter wri;
	// cout<<wri.write(User)<<endl;
	
}
int main()
{
	im::IM im_server;
	im_server.Init();
	im_server.Run();
	return 0;
}