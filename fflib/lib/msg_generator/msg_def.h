struct student_t 
{
    struct book_t 
    {
        int16_t pages;
        string contents;

    };
    float grade;
    int8_t age;
    map<string,  book_t>  books;
    vector<string>  friends;
    string name;

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