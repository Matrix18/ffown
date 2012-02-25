#include <iostream>
#include <cstdio>
using namespace std;

//#include "json_instream.h"
#include "json_outstream.h"
#include "rapidjson/stringbuffer.h" 
   
struct foo_t
{
	foo_t():a(1),b(2){}
	int a;
	int b;

	int decode(const json_value_t& jval_)
	{
		return 0;
	}

    string encode_json()
    {
        rapidjson::Document::AllocatorType allocator;
        rapidjson::StringBuffer            str_buff;
        json_value_t                       ret_json(rapidjson::kObjectType);

        this->encode_json_val(ret_json, allocator);
        rapidjson::Writer<rapidjson::StringBuffer> writer(str_buff, &allocator);
        ret_json.Accept(writer);
        string output(str_buff.GetString(), str_buff.Size());
        return output;
    }

	int encode_json_val(json_value_t& ret, rapidjson::Document::AllocatorType& allocator)
	{
		json_outstream_t out(allocator);
		json_value_t dest(rapidjson::kObjectType);

		out.encode("a", dest, a).encode("b", dest, b);
		//ret.SetObject();
		ret.AddMember("foo_t", dest, allocator);
		return 0;
	}
};


int main(int argc, char* argv[])
{
	//json_instream_t in("foo");
	json_value_t    jval;
	int8_t          dest = 'a';
	foo_t foo;
	//! in.decode("a", jval, dest).decode("b", jval, dest).decode("foo", jval, foo);

	//! rapidjson::Document encode;
	rapidjson::Document::AllocatorType allocator;
	json_value_t encode;
	encode.SetObject();
	foo.encode_json_val(encode, allocator);
	string output = foo.encode_json();

    /*
    rapidjson::Document::AllocatorType allocator2;
	encode.AddMember("hello", "world", allocator);
	encode.AddMember("hello2", "world2", allocator2);
    */

	cout << output << "\n";
	return 0;
}
