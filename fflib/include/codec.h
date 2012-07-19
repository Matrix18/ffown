//! 二进制序列化 
#ifndef _CODEC_H_
#define _CODEC_H_

#include <string>
#include <stdexcept>
#include <vector>
#include <iostream>
using namespace std;

#include "message.h"

namespace ff {

class bin_encoder_t;
class bin_decoder_t;

struct codec_i
{
    virtual ~codec_i(){}
    virtual string encode(uint16_t cmd_)         = 0;
    virtual void decode(const string& src_buff_) = 0;
};

struct codec_helper_i
{
    virtual ~codec_helper_i(){}
    virtual void encode(bin_encoder_t&) const = 0;
    virtual void decode(bin_decoder_t&)       = 0;
};

class bin_decoder_t
{
public:
    bin_decoder_t(){}
    explicit bin_decoder_t(const string& src_buff_):
        m_index_ptr(src_buff_.data()),
        m_remaindered(src_buff_.size())
    {
    }
    bin_decoder_t& init(const string& str_buff_)
    {
        m_index_ptr = str_buff_.data();
        m_remaindered = str_buff_.size();
        return *this;
    }

    template<typename T>
    bin_decoder_t& operator >> (T& dest_)
    {
        return copy_value(&dest_, sizeof(T));
    }

    bin_decoder_t& operator >> (string& dest_)
    {
        return copy_value(dest_);
    }

    template<typename T>
    bin_decoder_t& operator >>(vector<T>& dest_vt_)
    {
        uint32_t vt_size = 0;
        copy_value(&vt_size, sizeof(vt_size));

        for (size_t i = 0; i < vt_size; ++i)
        {
            T tmp;
            (*this) >> tmp;
            dest_vt_.push_back(tmp);
        }
        return *this;
    }
    
    template<typename T>
    bin_decoder_t& operator >>(codec_helper_i& dest_)
    {
        dest_.decode(*this);
        return *this;
    }
private:
    bin_decoder_t& copy_value(void* dest_, uint32_t var_size_)
    {
        if (m_remaindered < var_size_)
        {
            throw runtime_error("bin_decoder_t:msg size not enough");
        }
        ::memcpy(dest_, m_index_ptr, var_size_);
        m_index_ptr     += var_size_;
        m_remaindered  -= var_size_;
        return *this;
    }
    
    bin_decoder_t& copy_value(string& dest_)
    {
        uint32_t str_len = 0;
        copy_value(&str_len, sizeof(str_len));

        if (m_remaindered < str_len)
        {
            throw runtime_error("bin_decoder_t:msg size not enough");
        }
        dest_.assign((const char*)m_index_ptr, str_len);
        m_index_ptr     += str_len;
        m_remaindered  -= str_len;
        return *this;
    }
    
private:
    const char*  m_index_ptr;
    size_t       m_remaindered;
};

class bin_encoder_t
{
public:
    bin_encoder_t(){}
    explicit bin_encoder_t(uint16_t cmd_)
    {
        message_head_t h(cmd_);
        m_dest_buff.append((const char*)(&h), sizeof(h));
    }
    bin_encoder_t& init(uint32_t cmd_)
    {
        message_head_t h(cmd_);
        m_dest_buff.append((const char*)(&h), sizeof(h));
        return *this;
    }

    const string& get_buff() const { return m_dest_buff; }

    template<typename T>
    bin_encoder_t& operator << (const T& var_)
    {
        return copy_value((const char*)(&var_), sizeof(var_));
    }
    
    bin_encoder_t& operator << (const string& str_)
    {
        return copy_value(str_);
    }
    
    template<typename T>
    bin_encoder_t& operator <<(const vector<T>& src_vt_)
    {
        uint32_t vt_size = (uint32_t)src_vt_.size();
        copy_value((const char*)(&vt_size), sizeof(vt_size));

        for (uint32_t i = 0; i < vt_size; ++i)
        {
            (*this) << src_vt_[i];
        }
        return *this;
    }
    
    bin_encoder_t& operator <<(const codec_helper_i& dest_)
    {
        dest_.encode(*this);
        return *this;
    }
private:
    inline bin_encoder_t& copy_value(const string& str_)
    {
        size_t str_size = str_.size();
        copy_value((const char*)(&str_size), sizeof(str_size));
        copy_value(str_.data(), str_.size());
        return *this;
    }
    inline bin_encoder_t& copy_value(const void* src_, size_t size_)
    {
        m_dest_buff.append((const char*)(src_), size_);
        ((message_head_t*)m_dest_buff.data())->size += size_;
        return *this;
    }

private:
    string         m_dest_buff;
};

template<typename T>
struct  msg_traits_t
{
    msg_traits_t():msg_id(0){}
    uint16_t msg_id;
};

struct msg_i : public codec_i
{
    msg_i(const char* msg_name_):
        cmd(0),
        uuid(0),
        service_group_id(0),
        service_id(0),
        msg_name(msg_name_)
    {}
    
    void set(uint16_t group_id, uint16_t id_, uint32_t uuid_, uint16_t msg_id_)
    {
        service_group_id = group_id;
        service_id       = id_;
        uuid             = uuid_;
        msg_id           = msg_id_;
    }

    uint16_t cmd;
    uint16_t get_group_id()   const{ return service_group_id; }
    uint16_t get_service_id() const{ return service_id;       }
    uint32_t get_uuid()       const{ return uuid;             }
    
    uint16_t get_msg_id()     const{ return msg_id;           }
    const string& get_name()  const{ return msg_name;         }
    
    void     set_uuid(uint32_t id_)   { uuid = id_;  }
    void     set_msg_id(uint16_t id_) { msg_id = id_;}
    uint32_t uuid;
    uint16_t service_group_id;
    uint16_t service_id;
    uint16_t msg_id;
    string   msg_name;

    virtual string encode(uint16_t cmd_)
    {
        this->cmd = cmd_;
        return encode();
    }
    virtual string encode() = 0;
    bin_encoder_t& init_encoder()
    {
        return encoder.init(cmd)  << uuid << service_group_id << service_id<< msg_id << msg_name;
    }
    bin_encoder_t& init_encoder(uint16_t cmd_)
    {
        return encoder.init(cmd_) << uuid << service_group_id << service_id << msg_name;
    }
    bin_decoder_t& init_decoder(const string& buff_)
    {
        return decoder.init(buff_) >> uuid >> service_group_id >> service_id >> msg_id >> msg_name;
    }
    bin_decoder_t decoder;
    bin_encoder_t encoder;
};

struct base_msg_t: public msg_i
{
    base_msg_t(const char* name_):
        msg_i(name_)
    {}
    virtual string encode()
    {
        return init_encoder().get_buff();
    }
    virtual void decode(const string& src_buff_)
    {
        init_decoder(src_buff_);
    }
};

struct msg_tool_t: public base_msg_t
{
    msg_tool_t():
        base_msg_t("")
    {}
};

struct rpc_msg_cmd_e
{
    enum
    {
        CREATE_SERVICE_GROUP = 1,
        CREATE_SERVICE       = 2,
        REG_INTERFACE        = 3,
        CALL_INTERFACE       = 4,
        INTREFACE_CALLBACK   = 5,
    };
};

struct bool_ret_msg_t: public msg_i
{
    bool_ret_msg_t(const char* name_ = ""):
        msg_i(name_),
        value(false)
    {}
    virtual string encode()
    {
        return (init_encoder()<< value).get_buff();
    }
    virtual void decode(const string& src_buff_)
    {
        init_decoder(src_buff_) >> value;
    }
    bool value;
};

struct create_service_group_t
{
    struct in_t: public msg_i
    {
        in_t():
            msg_i("create_service_group_t::in")
        {}
        virtual string encode()
        {
            return (init_encoder()<< service_name).get_buff();
        }
        virtual void decode(const string& src_buff_)
        {
            init_decoder(src_buff_) >> service_name;
        }
        string service_name;
    };
    struct out_t: public msg_i
    {
        out_t():
            msg_i("create_service_group_t::out")
        {}
        virtual string encode()
        {
            return (init_encoder()<< service_id).get_buff();
        }
        virtual void decode(const string& src_buff_)
        {
            init_decoder(src_buff_) >> service_id;
        }
        uint16_t service_id;
    };
};

struct create_service_t
{
    struct in_t: public msg_i
    {
        in_t():
            msg_i("create_service_t::in")
        {}
        virtual string encode()
        {
            return (init_encoder()<< new_service_group_id << new_service_id).get_buff();
        }
        virtual void decode(const string& src_buff_)
        {
            init_decoder(src_buff_) >> new_service_group_id >> new_service_id;
        }
        uint16_t new_service_group_id;
        uint16_t new_service_id;
    };
    struct out_t: public bool_ret_msg_t
    {
        out_t(): bool_ret_msg_t("create_service_group_t::out"){}
    };
};

struct reg_interface_t
{
    struct in_t: public msg_i
    {
        in_t():
            msg_i("reg_interface_t::in")
        {}
        virtual string encode()
        {
            return (init_encoder()<< sgid << sid << in_msg_name << out_msg_name).get_buff();
        }
        virtual void decode(const string& src_buff_)
        {
            init_decoder(src_buff_) >> sgid >> sid >> in_msg_name >> in_msg_name;
        }
        uint16_t sgid;
        uint16_t sid;
        string in_msg_name;
        string out_msg_name;
    };
    struct out_t: public msg_i
    {
        out_t():
            msg_i("reg_interface_t::out")
        {}
        virtual string encode()
        {
            return (init_encoder()<< alloc_id).get_buff();
        }
        virtual void decode(const string& src_buff_)
        {
            init_decoder(src_buff_) >> alloc_id;
        }
        uint16_t alloc_id;
    };
};

}
#endif
