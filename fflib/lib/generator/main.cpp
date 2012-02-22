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

#include "msg_def.h"

class msg_handler_t
{
public:
    typedef int socket_ptr_t;
public:
    void handle(const student_t& s_,  socket_ptr_t sock_)
    {
        cout  << "msg_handler_t::handle:\n";
        cout  << "age:" << int(s_.age) << " grade:" << s_.grade << " friends:"<< s_.friends.size() << " name:"
              << s_.name << " mybook:" << s_.mybook.pages << " " << s_.mybook.contents << " note:" << s_.note.size() <<"\n";
              
        cout  << s_.encode_json() <<"\n";
    }
};


int main(int argc, char* argv[])
{
    try
    {
        string tmp = "{\"student_t\":{\"vt\":[{\"pages\":123,\"contents\":\"oh nice\"}],\"age\":12,\"grade\":1.2,\"name\":\"bible\",\"friends\":[\"a\",\"b\"],"
                     "\"mybook\":{\"pages\":123,\"contents\":\"oh nice\"},\"note\":{\"a\":123}}}";
        msg_handler_t xxx;
        msg_dispather_t<msg_handler_t, msg_handler_t::socket_ptr_t> p(xxx);
        p.dispath(tmp, 0);
    }
    catch(exception& e)
    {
        cout <<"e:"<< e.what() <<"\n";
    }
    cout <<"main end ok\n";
}