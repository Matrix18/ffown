
#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <iostream>
using namespace std;

#include "rapidjson/document.h"     // rapidjson's DOM-style API
#include "rapidjson/prettywriter.h" // for stringify JSON
#include "rapidjson/filestream.h"   // wrapper of C stream for prettywriter as output
//! using namespace rapidjson;

typedef runtime_error        msg_exception_t;
typedef rapidjson::Document  json_dom_t;
typedef rapidjson::Value     json_value_t;

typedef int int8 ;
typedef int int16;
typedef int int32;

struct student_t {
    struct book_t {
        int16 pages;
        string contents;
        int parse(const json_value_t& jval_) {
            char buff[128];
            const json_value_t& pages = jval_["pages"];
            if (false == pages.IsNumber())
            {
                snprintf(buff, sizeof(buff), "book_t::pages[int16] field needed");
                throw msg_exception_t(buff);
            }
            this->pages = pages.GetInt();
        
            const json_value_t& contents = jval_["contents"];
            if (false == contents.IsString())
            {
                snprintf(buff, sizeof(buff), "book_t::contents[string] field needed");
                throw msg_exception_t(buff);
            }
            this->contents = contents.GetString();
        
            return 0;
        }
    };
    book_t mybook;
    string name;
    float grade;
    int8 age;
    map<string, int8> note;
    vector<string> friends;
    int parse(const json_value_t& jval_) {
        char buff[128];
        const json_value_t& mybook = jval_["mybook"];
        if (false == mybook.IsObject())
        {
            snprintf(buff, sizeof(buff), "student_t::mybook[book_t] field needed");
            throw msg_exception_t(buff);
        }
        this->mybook.parse(mybook);
        
        const json_value_t& name = jval_["name"];
        if (false == name.IsString())
        {
            snprintf(buff, sizeof(buff), "student_t::name[string] field needed");
            throw msg_exception_t(buff);
        }
        this->name = name.GetString();
        
        const json_value_t& grade = jval_["grade"];
        if (false == grade.IsDouble())
        {
            snprintf(buff, sizeof(buff), "student_t::grade[float] field needed");
            throw msg_exception_t(buff);
        }
        this->grade = grade.GetDouble();
        
        const json_value_t& age = jval_["age"];
        if (false == age.IsNumber())
        {
            snprintf(buff, sizeof(buff), "student_t::age[int8] field needed");
            throw msg_exception_t(buff);
        }
        this->age = age.GetInt();
        
        const json_value_t& note = jval_["note"];
        if (false == note.IsObject())
        {
            snprintf(buff, sizeof(buff), "student_t::note[dictionary] field needed");
            throw msg_exception_t(buff);
        }
        rapidjson::Document::ConstMemberIterator it = note.MemberBegin();
        for (; it != note.MemberEnd(); ++it)
        {
            const  json_value_t& name = it->name;
            if (false == name.IsString())
            {
                snprintf(buff, sizeof(buff), "student_t::note[Object] key must [string]");
                throw msg_exception_t(buff);
            }
            const  json_value_t& val = it->value;
            if (false == val.IsNumber())
            {
                snprintf(buff, sizeof(buff), "student_t::note[Object] val must [int8]");
                throw msg_exception_t(buff);
            }
            string key_val; key_val = name.GetString();
            int8 tmp_val; tmp_val = val.GetInt();
            this->note[key_val] = tmp_val;
        }
        
        const json_value_t& friends = jval_["friends"];
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
                snprintf(buff, sizeof(buff), "student_t::friends field at[%u] must array", i);
                throw msg_exception_t(buff);
            }
            string tmp_val;
            tmp_val = val.GetString();
		    this->friends.push_back(tmp_val);
        }
        
        return 0;
    }
};

template<typename T, typename R>
class msg_dispather_t
{
    typedef R                    socket_ptr_t;
    typedef int (msg_dispather_t<T, R>::*reg_func_t)(const json_value_t&, socket_ptr_t);
public:
    msg_dispather_t(T& msg_handler_):
        m_msg_handler(msg_handler_)
    {
        
        m_reg_func["student_t"] = &msg_dispather_t<T, R>::student_t_dispacher;
            
    }

    int dispath(const string& json_, socket_ptr_t sock_)
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
        
    int student_t_dispacher(const json_value_t& jval_, socket_ptr_t sock_)
    {
        student_t ret_val;
        ret_val.parse(jval_);

        m_msg_handler.handle(ret_val, sock_);
        return 0;
    }
            
private:
    T&                      m_msg_handler;
    map<string, reg_func_t> m_reg_func;
};
        