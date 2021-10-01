#include<iostream>
#include"httplib.h"

int main()
{
    using namespace httplib;
    Server server;
     server.set_base_dir("./www");
     server.listen("0.0.0.0",2580);
     return 0;
}
