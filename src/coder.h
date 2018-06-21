#ifndef SOCK_CODER_H
#define SOCK_CODER_H

#include "buffer.h"
#include <cstring>
#include <stdexcept>
#include <type_traits>
#include <vector>

template <typename T>
class Raw
{
public:
    constexpr explicit Raw(const T &data)
        : m_data{reinterpret_cast<const char *>(&data)}
    {
    }

    constexpr const char *data() const { return m_data; }
    constexpr char *data() { return const_cast<char *>(m_data); }

private:
    const char *m_data;
};

template <typename T>
class Raw<T *>
{
    static_assert(sizeof(T) == -1, "pointer raw data");
};

class Var
{
public:
    constexpr Var(const char *data, std::size_t size)
        : m_data{data}, m_size{size}
    {
    }

    constexpr const char *data() const { return m_data; }
    constexpr char *data() { return const_cast<char *>(m_data); }
    constexpr std::size_t size() const { return m_size; }

private:
    const char *m_data;
    std::size_t m_size;
};

template <typename T>
constexpr Raw<T> raw(const T &data)
{
    return Raw<T>{data};
}

constexpr inline Var var(const char *data, std::size_t size)
{
    return Var{data, size};
}

template <typename Stream>
class Coder
{
public:
    explicit Coder(Stream &stream) : m_stream{&stream} {}

    template <typename T>
    Coder &operator<<(Raw<T> data)
    {
        m_stream->write(data.data(), sizeof(T));
        return *this;
    }

    Coder &operator<<(Var data)
    {
        *this << data.size();
        m_stream->write(data.data(), data.size());
        return *this;
    }

    Coder &operator<<(const Buffer &buf)
    {
        return *this << var(buf.data(), buf.size());
    }

    template <typename T>
    std::enable_if_t<std::is_integral<T>::value, Coder &> operator<<(T data)
    {
        return *this << raw(data);
    }

    Coder &operator<<(const std::string &data)
    {
        return *this << var(data.data(), data.size());
    }

    Coder &operator<<(const char *data)
    {
        return *this << var(data, std::strlen(data));
    }

    template <typename T>
    Coder &operator>>(Raw<T> data)
    {
        m_stream->read(data.data(), sizeof(T));
        return *this;
    }

    Coder &operator>>(Var data)
    {
        std::size_t size;
        *this >> size;
        if (data.size() < size)
            throw std::overflow_error{"buffer overflow"};

        m_stream->read(data.data(), size);
        return *this;
    }

    Coder &operator>>(Buffer &buf)
    {
        std::size_t size;
        *this >> size;
        buf.clear();
        m_stream->read(buf.write(size), size);
        return *this;
    }

    template <typename T>
    std::enable_if_t<std::is_integral<T>::value, Coder &> operator>>(T &data)
    {
        return *this >> raw(data);
    }

    Coder &operator>>(std::vector<char> &data)
    {
        std::size_t size;
        *this >> size;
        // if (size > 4096)
        //     std::cout << "WARNING: large size " << size << '\n';

        data.resize(size);
        m_stream->read(data.data(), size);
        return *this;
    }

    Coder &operator>>(std::string &data)
    {
        std::size_t size;
        *this >> size;
        // if (size > 4096)
        //     std::cout << "WARNING: large size " << size << '\n';

        data.resize(size);
        m_stream->read(&data[0], size);
        return *this;
    }

    const Stream &stream() const { return *m_stream; }
    Stream &stream() { return *m_stream; }

private:
    Stream *m_stream;
};

template <typename T>
Coder<T> coder(T &t)
{
    return Coder<T>{t};
}

#endif
