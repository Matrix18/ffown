
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
    float grade;
    map<string, int8> note;
    int8 age;
    vector<book_t> friends;
    string name;
    int parse(const json_value_t& jval_) {
        char buff[128];
        const json_value_t& grade = jval_["grade"];
        if (false == grade.IsDouble())
        {
            snprintf(buff, sizeof(buff), "student_t::grade[float] field needed");
            throw msg_exception_t(buff);
        }
        this->grade = grade.GetDouble();
        
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
        
        const json_value_t& age = jval_["age"];
        if (false == age.IsNumber())
        {
            snprintf(buff, sizeof(buff), "student_t::age[int8] field needed");
            throw msg_exception_t(buff);
        }
        this->age = age.GetInt();
        
        const json_value_t& friends = jval_["friends"];
        if (false == friends.IsArray())
        {
            snprintf(buff, sizeof(buff), "student_t::friends[array] field needed");
            throw msg_exception_t(buff);
        }
        for (rapidjson::SizeType i = 0; i < friends.Size(); i++)
        {
            const json_value_t& val = friends[i];
            if (false == val.IsObject())
            {
                snprintf(buff, sizeof(buff), "student_t::friends field at[%u] must array", i);
                throw msg_exception_t(buff);
            }
            book_t tmp_val;
            tmp_val.parse(val);
		    this->friends.push_back(tmp_val);
        }
        
        const json_value_t& name = jval_["name"];
        if (false == name.IsString())
        {
            snprintf(buff, sizeof(buff), "student_t::name[string] field needed");
            throw msg_exception_t(buff);
        }
        this->name = name.GetString();
        
        return 0;
    }
};