#include<iostream>
#include<json/json.h>
#include<string>

int main()
{
    Json::Value value;
    value["name"]="张三";
    value["age"]=(Json::Int64)18;
    value["score"]=88.88;

    Json::Value root;
    root.append(value);
    root.append(value);
    root.append(value);

    Json::StyledWriter writer;
    std::string str=writer.write(root);
    std::cout<<str<<std::endl;;

    Json::Value value1;
    Json::Reader reader;

    reader.parse(str,value1);

    int size=value1.size();
    for(int i=0;i<size;i++)
    {
        std::cout<< value1[i]["name"].asString()<<std::endl;
        std::cout<< value1[i]["age"].asInt()<<std::endl;
        std::cout<< value1[i]["score"].asFloat()<<std::endl;
    }
    
    return 0;

}
