#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
using namespace std;

#include "rapidjson/document.h"     // rapidjson's DOM-style API
#include "rapidjson/prettywriter.h" // for stringify JSON
#include "rapidjson/filestream.h"   // wrapper of C stream for prettywriter as output
using namespace rapidjson;

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <iostream>
#include <map>
#include <vector>
using namespace std;


struct student_t
{
    struct book_t
    {
        int16_t     pages;
        string      contents;
    };
    int8_t              age;
    float               grade;
    string              name;
    vector<string>      friends;
    map<string, book_t> books;
};

template<typename T, typename R>
class msg_dispather_t
{
    typedef runtime_error        msg_exception_t;
    typedef rapidjson::Document  json_dom_t;
    typedef rapidjson::Value     json_value_t;
    typedef R                    socket_ptr_t;
    typedef int (msg_dispather_t<T, R>::*reg_func_t)(const json_value_t&, socket_ptr_t);
public:
    msg_dispather_t(T& msg_handler_):
        m_msg_handler(msg_handler_)
    {
        m_reg_func["student_t"] = &msg_dispather_t<T, R>::student_t_dispacher;
    }
    int dispath(const string& json_, socket_ptr_t sock_);

private:
    int student_t_dispacher(const json_value_t& jval_, socket_ptr_t sock_)
    {
        student_t s_val;
        const json_value_t& age      = jval_["age"];
        const json_value_t& grade    = jval_["grade"];
        const json_value_t& name     = jval_["name"];
        const json_value_t& friends  = jval_["friends"];
        const json_value_t& books    = jval_["books"];
        char buff[512];

        if (false == age.IsNumber())
        {
            snprintf(buff, sizeof(buff), "student::age[int] field needed");
            throw msg_exception_t(buff);
        }
        s_val.age = age.GetInt();
        if (false == grade.IsDouble())
        {
            snprintf(buff, sizeof(buff), "student::grade[float] field needed");
            throw msg_exception_t(buff);
        }
        s_val.grade = grade.GetDouble();
        if (false == name.IsString())
        {
            snprintf(buff, sizeof(buff), "student::name[string] field needed");
            throw msg_exception_t(buff);
        }
        s_val.name = name.GetString();
        if (false == friends.IsArray())
        {
            snprintf(buff, sizeof(buff), "student::friends[Array] field needed");
            throw msg_exception_t(buff);
        }
        for (rapidjson::SizeType i = 0; i < friends.Size(); i++)
        {
            const json_value_t& val = friends[i];
            if (false == val.IsString())
            {
                snprintf(buff, sizeof(buff), "student::friends field at[%u] must string", i);
                throw msg_exception_t(buff);
            }
			s_val.friends.push_back(val.GetString());
        }
        if (false == books.IsObject())
        {
            snprintf(buff, sizeof(buff), "student::books[Object] field needed");
            throw msg_exception_t(buff);
        }
        rapidjson::Document::ConstMemberIterator it = books.MemberBegin();
        for (; it != books.MemberEnd(); ++it)
        {
            student_t::book_t book_val;
            const  json_value_t& name = it->name;
            if (false == name.IsString())
            {
                snprintf(buff, sizeof(buff), "student::books[Object] key must [string]");
                throw msg_exception_t(buff);
            }

            const  json_value_t& val = it->value;
            if (false == val.IsObject())
            {
                snprintf(buff, sizeof(buff), "student::books[Object] value must [Object]");
                throw msg_exception_t(buff);
            }

            const  json_value_t& book_pages   = val["pages"];
            const  json_value_t& book_contens = val["contents"];
            if (false == book_pages.IsNumber())
            {
                snprintf(buff, sizeof(buff), "student::books::pages[Number] field needed");
                throw msg_exception_t(buff);
            }
            book_val.pages    = book_pages.GetInt();
            if (false == book_contens.IsString())
            {
                snprintf(buff, sizeof(buff), "student::books::book_contens[String] field needed");
                throw msg_exception_t(buff);
            }
            book_val.contents = book_contens.GetString();
            s_val.books[name.GetString()] = book_val;
        }

        m_msg_handler.handle(s_val, sock_);
        return 0;
    }

private:
    T&                      m_msg_handler;
    map<string, reg_func_t> m_reg_func;
};

template<typename T, typename R>
int msg_dispather_t<T, R>::dispath(const string& json_, socket_ptr_t sock_)
{
    json_dom_t document;  // Default template parameter uses UTF8 and MemoryPoolAllocator.
    if (document.Parse<0>(json_.c_str()).HasParseError())
    {
        throw msg_exception_t("json format not right");
    }
    if (false == document.IsObject() && false == document.Empty())
    {
        throw msg_exception_t("json must has one field");
    }

    const json_value_t& val = document.MemberBegin()->name;
    const char* func_name   = val.GetString();
    reg_func_t func         = m_reg_func[func_name];

    if (NULL == func)
    {
        char buff[512];
        snprintf(buff, sizeof(buff), "msg not supported<%s>", func_name);
        throw msg_exception_t(buff);
        return -1;
    }

    (this->*func)(document.MemberBegin()->value, sock_);
    return 0;
}

class msg_handler_t
{
public:
    typedef int socket_ptr_t;
public:
    void handle(const student_t& s_,  socket_ptr_t sock_)
    {
        cout  << "msg_handler_t::handle:\n";
        cout  << "age:" << int(s_.age) << " grade:" << s_.grade << " friends:"<< s_.friends.size() << " name:"
              << s_.name << " books:" << s_.books.size() <<"\n";
    }
};

int main(int argc, char* argv[])
{
    try
    {
        string tmp = "{\"student_t\":{\"age\":123,\"grade\":1.2,\"name\":\"bible\",\"friends\":[\"a\",\"b\"],"
                     "\"books\":{\"bible\":{\"pages\":123,\"contents\":\"oh nice\"}}}}";
        msg_handler_t xxx;
        msg_dispather_t<msg_handler_t, msg_handler_t::socket_ptr_t> p(xxx);
        p.dispath(tmp, 0);
    }
    catch(exception& e)
    {
        cout <<"e:"<< e.what() <<"\n";
    }
}