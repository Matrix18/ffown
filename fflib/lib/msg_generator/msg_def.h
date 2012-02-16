#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <iostream>
using namespace std;

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h" //!  for stringify JSON
#include "rapidjson/filestream.h" //!    wrapper of C stream for prettywriter as output
//! using namespace rapidjson;

struct student_t 
{
    struct book_t 
    {
        int16_t pages;
        string contents;

    };
    book_t mybook;
    string name;
    float grade;
    int8_t age;
    map<string, string>  note;
    vector<string>  friends;

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
   int student_t_dispacher(const json_value_t& jval_, socket_ptr_t sock_);

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
    typename map<string, reg_func_t>::const_iterator it = m_reg_func.find(func_name);

    if (it == m_reg_func.end())
    {
        char buff[512];
        snprintf(buff, sizeof(buff), "msg not supported<%s>", func_name);
        throw msg_exception_t(buff);
        return -1;
    }
    reg_func_t func = it->second;

    (this->*func)(document.MemberBegin()->value, sock_);
    return 0;
}
template<typename T, typename R>
int msg_dispather_t<T, R>::student_t_dispacher(const json_value_t& jval_, socket_ptr_t sock_)
{
    student_t ret_val;
    char buff[512];

    const json_value_t&     mybook = jval_["mybook"];
    if (false == mybook.IsObject())
    { 
        snprintf(buff, sizeof(buff), "student_t::mybook[book_t] field needed");
        throw msg_exception_t(buff);
    }
    const  json_value_t& _mybook_pages = mybook["pages"];
    if (false == _mybook_pages.IsNumber())
    {
        snprintf(buff, sizeof(buff), "student_t::mybook[book_t]->pages key must [int16]");
        throw msg_exception_t(buff);
    }
    ret_val.mybook.pages = _mybook_pages.GetInt();
    const  json_value_t& _mybook_contents = mybook["contents"];
    if (false == _mybook_contents.IsString())
    {
        snprintf(buff, sizeof(buff), "student_t::mybook[book_t]->contents key must [string]");
        throw msg_exception_t(buff);
    }
    ret_val.mybook.contents = _mybook_contents.GetString();

    const json_value_t&       name = jval_["name"];
    if (false == name.IsString())
    { 
        snprintf(buff, sizeof(buff), "student_t::name[string] field needed");
        throw msg_exception_t(buff);
    }
    ret_val.name = name.GetString();
 
    const json_value_t&      grade = jval_["grade"];
    if (false == grade.IsDouble())
    { 
        snprintf(buff, sizeof(buff), "student_t::grade[float] field needed");
        throw msg_exception_t(buff);
    }
    ret_val.grade = grade.GetDouble();
 
    const json_value_t&        age = jval_["age"];
    if (false == age.IsNumber())
    { 
        snprintf(buff, sizeof(buff), "student_t::age[int8] field needed");
        throw msg_exception_t(buff);
    }
    ret_val.age = age.GetInt();
 
    const json_value_t&       note = jval_["note"];
    if (false == note.IsObject())
    { 
        snprintf(buff, sizeof(buff), "student_t::note[dictionary] field needed");
        throw msg_exception_t(buff);
    }
    rapidjson::Document::ConstMemberIterator it = note.MemberBegin();
    for (; it != note.MemberEnd(); ++it)
    {
        const json_value_t& _name = it->name;
        if (false == _name.IsString())
        {
            snprintf(buff, sizeof(buff), "student_t::note[dictionary] key must [string]");
            throw msg_exception_t(buff);
        }
        const  json_value_t& _val = it->value;
        if (false == _val.IsString())
        {
            snprintf(buff, sizeof(buff), "student_t::note[dictionary] value must [string]");
            throw msg_exception_t(buff);
        }
        ret_val.note[_name.GetString()] = _val.GetString();
    }

    const json_value_t&    friends = jval_["friends"];
    if (false == friends.IsArray())
    { 
        snprintf(buff, sizeof(buff), "student_t::friends[array] field needed");
        throw msg_exception_t(buff);
    }
    for (rapidjson::SizeType i = 0; i < friends.Size(); i++)
    {
        const json_value_t& val = friends[i];
        if (false == val.IsString())
        {
            snprintf(buff, sizeof(buff), "student_t::friends[array] field at[%u] must string", i);
            throw msg_exception_t(buff);
        }

        ret_val.friends.push_back(val.GetString());
    }
    m_msg_handler.handle(ret_val, sock_);
    return 0;
}
