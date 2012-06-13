//! 二进制序列化 
#ifndef _CODEC_H_
#define _CODEC_H_

#include <string>
#include <stdexcept>
#include <vector>
#include <iostream>
using namespace std;

#include "message.h"

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


struct msg_i : public codec_i
{
    msg_i(const char* msg_name_):
    service_group_id(0),
    service_id(0),
    uuid(0),
    msg_name(msg_name_)
    {}
    
    void set(uint16_t group_id, uint16_t id_, uint32_t uuid_)
    {
        service_group_id = group_id;
        service_id       = id_;
        uuid             = uuid_;
    }

    uint16_t get_group_id()   const{ return service_group_id; }
    uint16_t get_service_id() const{ return service_id;       }
    uint32_t get_uuid()       const{ return uuid;             }
    uint16_t service_group_id;
    uint16_t service_id;
    uint32_t uuid;
    string   msg_name;

    bin_encoder_t& init_encoder(uint16_t cmd_)
    {
        return encoder.init(cmd_) << service_group_id << service_id << uuid << msg_name;
    }
    bin_decoder_t& init_decoder(const string& buff_)
    {
        return decoder.init(buff_) >> service_group_id >> service_id >> uuid >> msg_name;
    }
    bin_decoder_t decoder;
    bin_encoder_t encoder;
};

#endif
