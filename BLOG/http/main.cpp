#include"db.hpp"
#include"httplib.h"

using namespace httplib;
MYSQL *mysql=blog_system::MysqlInit();
    blog_system::TableBlog *table_blog=new blog_system::TableBlog(mysql);
    blog_system::TableTag *table_tag=new blog_system::TableTag(mysql);;
void InsertBlog(const Request &req,Response &rsp)
{
    Json::Reader reader;
    Json::Value blog;
    Json::FastWriter writer;
    Json::Value errmsg;
    bool ret=reader.parse(req.body,blog);
    if(ret==false){
        printf("InsertBlog parse blog json filed!\n");
        rsp.status=400;
        errmsg["ok"]=false;
        errmsg["reason"]="parse blog json filed!";
        rsp.set_content(writer.write(errmsg),"application/json");
        return;
    }
    ret=table_blog->Insert(blog);
    if(ret==false)
    {
         printf("InsertBlog insert into database filed!\n");

        rsp.status=500;
        return ;
    }
    rsp.status=200;
    return;
}
void DeleteBlog(const Request &req,Response &rsp)
{

    // /blog//123   /blog/(\d+) req.matches[0]=/blog//123
    int blog_id=std::stoi(req.matches[1]);
    bool ret= table_blog->Delete(blog_id);
     if(ret==false)
    {
        printf("DeleteBlog delete from database filed!\n");
        rsp.status=500;
        return ;
    }
    rsp.status=200;
    return;

}
void UpdateBlog(const Request &req,Response &rsp)
{
  int blog_id=std::stoi(req.matches[1]);
  Json::Value blog;
  Json::Reader reader;
  bool ret=reader.parse(req.body,blog);
  if(ret==false)
  {
    printf("UpdatBlog parse blog json filed!\n");
    rsp.status=400;
    return;
  }
  blog["id"]=blog_id;
  ret=table_blog->Update(blog);
  if(ret==false)
    {
        printf("UpdatBlog elete from database filed!\n");
        rsp.status=500;
        return ;
    }
    rsp.status=200;
    return;
}
void GetAllBlog(const Request &req,Response &rsp)
{
    Json::Value blogs;
    bool ret=table_blog->GetAll(&blogs);
     if(ret==false)
    {
        printf("GetAll from database filed!\n");
        rsp.status=500;
        return ;
    }
    Json::FastWriter writer;
    rsp.set_content(writer.write(blogs),"application/json");
    rsp.status=200;
    return;

}
void GetOneBlog(const Request &req,Response &rsp)
{
    Json::Value blogs;
    bool ret=table_blog->GetOne(&blogs);
     if(ret==false)
    {
        printf("GetOne from database filed!\n");
        rsp.status=500;
        return ;
    }
    Json::FastWriter writer;
    rsp.set_content(writer.write(blogs),"application/json");
    rsp.status=200;
    return;
}

void InsertTag(const Request &req,Response &rsp)
{
    Json::Reader reader;
    Json::Value tag;
    Json::FastWriter writer;
    Json::Value errmsg;
    bool ret=reader.parse(req.body,tag);
    if(ret==false){
        printf("InsertBlog parse blog json filed!\n");

        rsp.status=400;
        errmsg["ok"]=false;
        errmsg["reason"]="parse blog json filed!";
        rsp.set_content(writer.write(errmsg),"application/json");
        return ;
    }
    ret=table_tag->Insert(tag);
    if(ret==false)
    {
         printf("InsertBlog insert into database filed!\n");

        rsp.status=500;
        return ;
    }
    rsp.status=200;
    return;
}
void DeleteTag(const Request &req,Response &rsp)
{

    // /blog//123   /blog/(\d+) req.matches[0]=/blog//123
    int tag_id=std::stoi(req.matches[1]);
    bool ret= table_tag->Delete(tag_id);
     if(ret==false)
    {
        printf("DeleteBlog delete from database filed!\n");
        rsp.status=500;
        return ;
    }
    rsp.status=200;
    return;

}
void UpdateTag(const Request &req,Response &rsp)
{
    int tag_id=std::stoi(req.matches[1]);
    Json::Value tag;
     Json::Reader reader;
    bool ret=reader.parse(req.body,tag);
     if(ret==false)
    {
    printf("UpdatBlog parse blog json filed!\n");
    rsp.status=400;
    return;
     }
    tag["id"]=tag_id;
    ret=table_tag->Update(tag);
    if(ret==false)
    {
        printf("UpdatBlog elete from database filed!\n");
        rsp.status=500;
        return ;
    }
    rsp.status=200;
    return;
}
void GetAllTag(const Request &req,Response &rsp)
{
      Json::Value tags;
    bool ret=table_blog->GetAll(&tags);
     if(ret==false)
    {
        printf("GetAll from database filed!\n");
        rsp.status=500;
        return ;
    }
    Json::FastWriter writer;
    rsp.set_content(writer.write(tags),"application/json");
    rsp.status=200;
    return;
}
void GetOneTag(const Request &req,Response &rsp)
{
    Json::Value tag;
    bool ret=table_tag->GetOne(&tag);
     if(ret==false)
    {
        printf("GetOne from database filed!\n");
        rsp.status=500;
        return ;
    }
    Json::FastWriter writer;
    rsp.set_content(writer.write(tag),"application/json");
    rsp.status=200;
    return;
}
int main()
{

    
    Server server;
    server.set_base_dir("./www");//设置url中的资源和相对根目录，并且在自动请求的时候，自动添加index.html
    //博客信息的增删改查
    server.Post("/blog",InsertBlog);
    server.Delete(R"(/blog(\d+))",DeleteBlog);//正则表达式是：\d-匹配数字字符，+表示匹配前面的字符一次或多次
    server.Put(R"(/blog(\d+))",UpdateBlog);
    server.Get("/blog",GetAllBlog);
    server.Get(R"(/blog(\d+))",GetOneBlog);

    server.Post("/tag",InsertTag);
    server.Delete(R"(/tag(\d+))",DeleteTag);//正则表达式是：\d-匹配数字字符，+表示匹配前面的字符一次或多次
    server.Put(R"(/tag(\d+))", UpdateTag);
    server.Get("/tag",GetAllTag);
    server.Get(R"(/tag(\d+))",GetOneTag);

    server.listen("0.0.0.0",2580);


}