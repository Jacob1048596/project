#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include"httplib.h"
void test(const httplib::Request& req,httplib::Response& res)
{
	(void)req;
	res.set_header("Set-Cookie","JSESSION");
	res.set_header("Content-type","application/json");
	res.set_content("hello","text/plain");
}
int main()
{
	httplib::Server ser;
	
	ser.Get("/a",test);

	ser.listen("0.0.0.0",8888);
	

	return 0;
	
}

