#pragma once
#include "Common.hpp"
#include <string>
#include <charconv>
#include <expected>
#include <system_error>

DOTL_NAMESPACE_BEGIN(dotl)

template<typename T>
struct ParseRes {
    enum Err {
        None, InvalidFormat, OutOfRange
    };

private:
    Err m_err;
    T m_val;
    ParseRes (Err error, T value): m_val(value),m_err(error) {}
public:

    static ParseRes Success(T val) { return ParseRes{Err::None, val}; }
    static ParseRes Failure(Err error_category) { return ParseRes{error_category, T{}}; }

    bool error() { return m_err!=Err::None; }
    T value() { return (this->error())? 0 : m_val; }
    Err error_category() { return m_err; }
};


// struct parsable-string: std::string compatible type for parsing
// values from strings easily with methods out of the box
struct PString
{
    std::string m_data;

    std::string string() { return m_data; }
    operator std::string() { return this->string(); }

    template <typename T>
    ParseRes<T> impl_parse() {
        T v;
        auto parsed = std::from_chars(m_data.data(), m_data.data()+m_data.size(), v);
        // handle parse fail
        if (parsed.ec != std::errc{}) {
            return ParseRes<T>::Failure(ParseRes<T>::Err::InvalidFormat);
        } else {
            return ParseRes<T>::Success(v);
        }
    }

public:

    template<typename T>
    ParseRes<T> parse() = delete;
};

// 
template<> inline
ParseRes<int32> PString::parse<int32>() { return impl_parse<int32>(); }
template<> inline
ParseRes<fp64> PString::parse<fp64>() { return impl_parse<fp64>(); }



DOTL_NAMESPACE_END()
