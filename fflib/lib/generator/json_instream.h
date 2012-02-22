#ifndef _JSON_INSTREAM_H_
#define _JSON_INSTREAM_H_

#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
using namespace std;

#include "rapidjson/document.h"     // rapidjson's DOM-style API                                                                                             
#include "rapidjson/prettywriter.h" // for stringify JSON
#include "rapidjson/filestream.h"   // wrapper of C stream for prettywriter as output

typedef runtime_error       msg_exception_t;
typedef rapidjson::Document json_dom_t;
typedef rapidjson::Value    json_value_t;

class json_instream_t
{
public:
    json_instream_t(const char* struct_name_);

    json_instream_t& decode(const char* filed_name_, const json_value_t& jval_, int8_t& dest_);
    json_instream_t& decode(const char* filed_name_, const json_value_t& jval_, uint8_t& dest_);
    json_instream_t& decode(const char* filed_name_, const json_value_t& jval_, int16_t& dest_);
    json_instream_t& decode(const char* filed_name_, const json_value_t& jval_, uint16_t& dest_);
    json_instream_t& decode(const char* filed_name_, const json_value_t& jval_, int32_t& dest_);
    json_instream_t& decode(const char* filed_name_, const json_value_t& jval_, uint32_t& dest_);
    json_instream_t& decode(const char* filed_name_, const json_value_t& jval_, int64_t& dest_);
    json_instream_t& decode(const char* filed_name_, const json_value_t& jval_, uint64_t& dest_);
    json_instream_t& decode(const char* filed_name_, const json_value_t& jval_, bool& dest_);
    json_instream_t& decode(const char* filed_name_, const json_value_t& jval_, float& dest_);
    json_instream_t& decode(const char* filed_name_, const json_value_t& jval_, string& dest_);

    template<typename T>
    json_instream_t& decode(const char* filed_name_, const json_value_t& jval_, T& dest_);
    template<typename T>
    json_instream_t& decode(const char* filed_name_, const json_value_t& jval_, vector<T>& dest_);
    template<typename T, typename R>
    json_instream_t& decode(const char* filed_name_, const json_value_t& jval_, map<T, R>& dest_);
private:
    char            m_err_buff[128];
    const char*     m_struct_name;
};

template<typename T>
json_instream_t& json_instream_t::decode(const char* filed_name_, const json_value_t& jval_, T& dest_)
{
    if (false == jval_.IsObject())
    {
        snprintf(m_err_buff, sizeof(m_err_buff), "%s::%s[Object] field needed", m_struct_name, filed_name_);
        throw msg_exception_t(m_err_buff);
    }

    dest_.parse(jval_);
    return *this;
}

template<typename T>
json_instream_t& json_instream_t::decode(const char* filed_name_, const json_value_t& jval_, vector<T>& dest_)
{
    if (false == jval_.IsArray())
    {
        snprintf(m_err_buff, sizeof(m_err_buff), "%s::%s[Array] field needed", m_struct_name, filed_name_);
        throw msg_exception_t(m_err_buff);
    }

    for (rapidjson::SizeType i = 0; i < jval_.Size(); i++)
    {
        T tmp_val;
        this->decode(filed_name_, jval_[i], tmp_val);
        dest_.push_back(tmp_val);
    }

    return *this;
}

template<typename T, typename R>
json_instream_t& json_instream_t::decode(const char* filed_name_, const json_value_t& jval_, map<T, R>& dest_)
{
    if (false == jval_.IsObject())
    {
        snprintf(m_err_buff, sizeof(m_err_buff), "%s::%s[Dictionary] field needed", m_struct_name, filed_name_);
        throw msg_exception_t(m_err_buff);
    }

    rapidjson::Document::ConstMemberIterator it = jval_.MemberBegin();
    for (; it != jval_.MemberEnd(); ++it)
    {
        T key;
        R val;
        this->decode(filed_name_, it->name, key);
        this->decode(filed_name_, it->value, val);
        dest_[key] = val;
    }

    return *this;
}


json_instream_t::json_instream_t(const char* struct_name_):
    m_struct_name(struct_name_)
{
}

json_instream_t& json_instream_t::decode(const char* filed_name_, const json_value_t& jval_, int8_t& dest_)
{
    if (false == jval_.IsInt())
    {
        snprintf(m_err_buff, sizeof(m_err_buff), "%s::%s[int8_t] field needed", m_struct_name, filed_name_);
        throw msg_exception_t(m_err_buff);
    }

    dest_ = jval_.GetInt();
    return *this;
}

json_instream_t& json_instream_t::decode(const char* filed_name_, const json_value_t& jval_, uint8_t& dest_)
{
    if (false == jval_.IsUint())
    {
        snprintf(m_err_buff, sizeof(m_err_buff), "%s::%s[uint8_t] field needed", m_struct_name, filed_name_);
        throw msg_exception_t(m_err_buff);
    }

    dest_ = jval_.GetUint();
    return *this;
}

json_instream_t& json_instream_t::decode(const char* filed_name_, const json_value_t& jval_, int16_t& dest_)
{
    if (false == jval_.IsInt())
    {
        snprintf(m_err_buff, sizeof(m_err_buff), "%s::%s[int16_t] field needed", m_struct_name, filed_name_);
        throw msg_exception_t(m_err_buff);
    }

    dest_ = jval_.GetInt();
    return *this;
}

json_instream_t& json_instream_t::decode(const char* filed_name_, const json_value_t& jval_, uint16_t& dest_)
{
    if (false == jval_.IsUint())
    {
        snprintf(m_err_buff, sizeof(m_err_buff), "%s::%s[uint16_t] field needed", m_struct_name, filed_name_);
        throw msg_exception_t(m_err_buff);
    }

    dest_ = jval_.GetUint();
    return *this;
}

json_instream_t& json_instream_t::decode(const char* filed_name_, const json_value_t& jval_, int32_t& dest_)
{
    if (false == jval_.IsInt())
    {
        snprintf(m_err_buff, sizeof(m_err_buff), "%s::%s[int32_t] field needed", m_struct_name, filed_name_);
        throw msg_exception_t(m_err_buff);
    }

    dest_ = jval_.GetInt();
    return *this;
}

json_instream_t& json_instream_t::decode(const char* filed_name_, const json_value_t& jval_, uint32_t& dest_)
{
    if (false == jval_.IsUint())
    {
        snprintf(m_err_buff, sizeof(m_err_buff), "%s::%s[int32_t] field needed", m_struct_name, filed_name_);
        throw msg_exception_t(m_err_buff);
    }

    dest_ = jval_.GetUint();
    return *this;
}

json_instream_t& json_instream_t::decode(const char* filed_name_, const json_value_t& jval_, int64_t& dest_)
{
    if (false == jval_.IsInt64())
    {
        snprintf(m_err_buff, sizeof(m_err_buff), "%s::%s[int64_t] field needed", m_struct_name, filed_name_);
        throw msg_exception_t(m_err_buff);
    }

    dest_ = jval_.GetInt64();
    return *this;
}

json_instream_t& json_instream_t::decode(const char* filed_name_, const json_value_t& jval_, uint64_t& dest_)
{
    if (false == jval_.IsUint64())
    {
        snprintf(m_err_buff, sizeof(m_err_buff), "%s::%s[uint64_t] field needed", m_struct_name, filed_name_);
        throw msg_exception_t(m_err_buff);
    }

    dest_ = jval_.GetUint64();
    return *this;
}

json_instream_t& json_instream_t::decode(const char* filed_name_, const json_value_t& jval_, bool& dest_)
{
    if (false == jval_.IsBool())
    {
        snprintf(m_err_buff, sizeof(m_err_buff), "%s::%s[bool] field needed", m_struct_name, filed_name_);
        throw msg_exception_t(m_err_buff);
    }

    dest_ = jval_.GetBool();
    return *this;
}

json_instream_t& json_instream_t::decode(const char* filed_name_, const json_value_t& jval_, float& dest_)
{
    if (false == jval_.IsDouble())
    {
        snprintf(m_err_buff, sizeof(m_err_buff), "%s::%s[float] field needed", m_struct_name, filed_name_);
        throw msg_exception_t(m_err_buff);
    }

    dest_ = (float)jval_.GetDouble();
    return *this;
}

json_instream_t& json_instream_t::decode(const char* filed_name_, const json_value_t& jval_, string& dest_)
{
    if (false == jval_.IsString())
    {
        snprintf(m_err_buff, sizeof(m_err_buff), "%s::%s[string] field needed", m_struct_name, filed_name_);
        throw msg_exception_t(m_err_buff);
    }

    dest_ = jval_.GetString();
    return *this;
}

#endif
