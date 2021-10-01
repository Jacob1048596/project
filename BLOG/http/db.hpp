#include<iostream>
#include<mysql/mysql.h>
#include<json/json.h>
#include<mutex>

#define MYSQL_HOST "127.0.0.1"
#define MYSQL_USER "root"
#define MYSQL_PSWD ""
#define MYSQL_DB "db_blog1"
namespace blog_system
{
    static std::mutex g_mutex;
    MYSQL *MysqlInit()//初始化
    {
        MYSQL *mysql;
         //初始化
         mysql=mysql_init(NULL);
        if(mysql==NULL)
         {
         printf("init mysql error\n");
             return NULL;
        }
         //连接服务器
        if(mysql_real_connect(mysql,MYSQL_HOST,MYSQL_USER,MYSQL_PSWD,NULL,0,NULL,0)==NULL)
        {
            printf("connect mysql error :%s\n",mysql_error(mysql));
            mysql_close(mysql);
            return NULL;
        }
        //选择字符集
        if(mysql_set_character_set(mysql,"utf8")!=0)
        {
            printf("connect mysql error :%s\n",mysql_error(mysql));
            mysql_close(mysql);
            return NULL;
        }
        //选择数据库
        if(mysql_select_db(mysql,MYSQL_DB)!=0)
        {
            printf("select db error :%s\n",mysql_error(mysql));
            return NULL;
        }
        return mysql;
    }
    void MysqlRelease(MYSQL *mysql)//销毁句柄
    {
        if(mysql)
        {
            mysql_close(mysql);
        }
        return ;
    }
    bool MysqlQuery(MYSQL *mysql,const char* sql)//执行语句的共有接口
    {
        int ret=mysql_query(mysql,sql);
        if(ret!=0)
        {
            printf("squery fild [%s] error faid:%s\n",sql,mysql_error(mysql));
            return false;
        }
        return true;
    }

    class TableBlog
    {
        public:
        TableBlog(MYSQL *mysql):_mysql(mysql){}
        bool Insert(Json::Value& blog)//从blog中取出博客信息，组指sql语句，将数据插入数据库
        {
            #define INSERT_BLOG "insert tb_blog values(null,'%d','%s','%s',now());"
            int len=blog["content"].asString().size()+4096;
            char *temp=(char*)malloc(len);
            sprintf(temp,INSERT_BLOG,
            blog["tag_id"].asInt(),
            blog["title"].asCString(),
            blog["connect"].asCString());
            bool ret=MysqlQuery(_mysql,temp);
            free(temp);
            return ret;
        }
        bool Delete(int blog_id)//根据博客id删除博客
        {
            #define DELETE_BLOG "delete from tb_blog where id=%d;"
            char temp[1024]={0};
            sprintf(temp,DELETE_BLOG,blog_id);
             bool ret=MysqlQuery(_mysql,temp);
            return ret;
        }
        bool Update(Json::Value& blog)//从blog中取出博客信息，组指sql语句，更新数据
        {
            #define UPDATE_BLOG "update tb_blog set tag_id=%d,title='%s',content='%s' where id=%d;"
            int len=blog["content"].asString().size()+4096;
            char *temp=(char*)malloc(len);
             sprintf(temp,UPDATE_BLOG,
            blog["tag_id"].asInt(),
            blog["title"].asCString(),
            blog["connect"].asCString(),
            blog["id"].asInt());
            bool ret=MysqlQuery(_mysql,temp);
            free(temp);
            return ret;

        }
        bool GetAll(Json::Value* blogs)//通过blog返回所有的博客信息（列表显示，不包含正文）
        {
             #define GETALL_BLOG "select id,tag_id,title,ctime from tb_blog"

            _mutex.lock();
            bool ret=MysqlQuery(_mysql,GETALL_BLOG);//执行查询语句
            if(ret==false)
            {
                _mutex.unlock();
                return false;
            }

            MYSQL_RES *res=mysql_store_result(_mysql);//保存结果集
            _mutex.unlock();
            if(res==NULL)
            {
                printf("store all blog faid error :%s\n",mysql_error(_mysql));
                return false;
            }
            //遍历结果集
            int row_num=mysql_num_rows(res);
            for(int i=0;i<row_num;i++)
            {
                MYSQL_ROW row=mysql_fetch_row(res);
                Json::Value blog;
                blog["id"]=std::stoi(row[0]);
                blog["tag_id"]=std::stoi(row[1]);
                blog["title"]=row[2];
                blog["ctime"]=row[3];
                blogs->append(blog);//添加json数组元素
            }
            mysql_free_result(res);
            return true;
        }
        bool GetOne(Json::Value* blog)//返回单个博客信息，包含正文
        {
            #define GETONE_BLOG "select tag_id,title,content,ctime from tb_blog where id=%d;"
            char temp[1024]={0};
            sprintf(temp,GETONE_BLOG, (*blog)["id"].asInt());//
            _mutex.lock();
             bool ret=MysqlQuery(_mysql,temp);
            if(ret==false)
            {
                _mutex.unlock();
                return false;
            }
            MYSQL_RES *res=mysql_store_result(_mysql);//保存结果集
            _mutex.unlock();
            if(res==NULL)
            {
                printf("store all blog faid error :%s\n",mysql_error(_mysql));
                return false;
            }
            int row_num=mysql_num_rows(res);
            if(row_num!=1)
            {
                printf("get one blog faid error :%s\n",mysql_error(_mysql));
                mysql_free_result(res);
                return false;
            }
            MYSQL_ROW row =mysql_fetch_row(res);
            (*blog)["tag_id"]=std::stoi(row[0]);
            (*blog)["title"]=row[1];
            (*blog)["content"]=row[2];
            (*blog)["ctime"]=row[3];

            mysql_free_result(res); 
            return true;
        }
        private:
        MYSQL *_mysql;
        std::mutex _mutex;
        
    };
    class TableTag
    {
        public:
        TableTag(MYSQL *mysql):_mysql(mysql){}
        bool Insert(Json::Value &tag)
        {
            #define INSERT_TAG "insert tb_tag values(null,'%s');"
            char temp[1024]={0};
            sprintf(temp,INSERT_TAG,
            tag["tag_name"].asCString());
            bool ret=MysqlQuery(_mysql,temp);
            return ret;
        }
        bool Delete(int tag_id)
        {
            #define DELETE_TAG "delete from tb_tag where tag_id=%d;"
            char temp[1024]={0};
            sprintf(temp,DELETE_TAG,tag_id);
            bool ret=MysqlQuery(_mysql,temp);
            return ret;
        }
        bool Update(Json::Value &tag)
        {
            #define UPDATE_TAG "update tb_tag set tag_name='%s'where tag_id=%d;"
            char temp[1024]={0};
            sprintf(temp,UPDATE_TAG,tag["tag_name"].asCString(),tag["tag_id"].asInt());
            bool ret=MysqlQuery(_mysql,temp);
            return ret;
        }
        bool GetAll(Json::Value *tags)
        {
           
            #define GETALL_TAG "select tag_id,tag_name from tb_tag;"
             //std::cout<<"test"<<std::endl;
            g_mutex.lock();
             //std::cout<<"test"<<std::endl;
            bool ret=MysqlQuery(_mysql,GETALL_TAG);//执行查询语句
            if(ret==false)
            {
                 //std::cout<<"test"<<std::endl;
                g_mutex.unlock();
                // std::cout<<"test"<<std::endl;
                return false;
            }
             //std::cout<<"test"<<std::endl;
            MYSQL_RES *res=mysql_store_result(_mysql);//保存结果集
             //std::cout<<"test"<<std::endl;
            g_mutex.unlock();
             if(res==NULL)
             {
                printf("store all blog faid error :%s\n",mysql_error(_mysql));
                return false;
             }
             //遍历结果集
            int row_num=mysql_num_rows(res);
           
            for(int i=0;i<row_num;i++)
            {
                MYSQL_ROW row=mysql_fetch_row(res);
                Json::Value tag;
                tag["tag_id"]=std::stoi(row[0]);
                tag["tag_name"]=row[1];
                
                tags->append(tag);//添加json数组元素
            }
            mysql_free_result(res);
            return true;
        }
        bool GetOne(Json::Value *tag)
        {
            #define GETONE_TAG "select tag_name from tb_tag where tag_id=%d;"
            char temp[1024]={0};
            sprintf(temp,GETONE_TAG, (*tag)["tag_id"].asInt());//

           
            g_mutex.lock();
             bool ret=MysqlQuery(_mysql,temp);
            if(ret==false)
            {
                g_mutex.unlock();
                return false;
            }
            MYSQL_RES *res=mysql_store_result(_mysql);//保存结果集
            g_mutex.unlock();
            if(res==NULL)
            {
                printf("store one tag faid error :%s\n",mysql_error(_mysql));
                return false;
            }
            int row_num=mysql_num_rows(res);
            if(row_num!=1)
            {
                printf("get one tag faid error :%s\n",mysql_error(_mysql));
                mysql_free_result(res);
                return false;
            }
           
             MYSQL_ROW row =mysql_fetch_row(res);
            (*tag)["tag_name"]=row[0];
             printf("%s\n",row[0]);
             mysql_free_result(res); 
            return true;
        }
        private:
        MYSQL * _mysql; 
        std::mutex _mutex;
    };
}
