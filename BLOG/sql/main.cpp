#include"db.hpp"
int main()
{
    MYSQL *mysql=blog_system::MysqlInit();
    blog_system::TableBlog table_blog(mysql);
    blog_system::TableTag table_tag(mysql);
    Json::Value tag;
    Json::Value blog;
    //tag["tag_id"]=1;
    //tag["tag_name"]="C";
    //table_tag.Insert(tag);
    //table_tag.GetAll(&tag);
    //tag["tag_id"]=1;
    Json::StyledWriter writer;
    table_blog.GetAll(&blog);
    std::cout<<writer.write(blog)<<std::endl;
    return 0;
}
