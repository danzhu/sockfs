#ifndef SOCK_SOCKET_H
#define SOCK_SOCKET_H

#include "fd.h"
#include "lib.h"
#include <cstdint>
#include <cstring>
#include <iostream>
#include <sstream>
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
    constexpr Var(const char *data, size_t size) : m_data{data}, m_size{size} {}

    constexpr const char *data() const { return m_data; }
    constexpr char *data() { return const_cast<char *>(m_data); }
    constexpr size_t size() const { return m_size; }

private:
    const char *m_data;
    size_t m_size;
};

template <typename T>
constexpr Raw<T> raw(const T &data)
{
    return Raw<T>{data};
}

constexpr inline Var var(const char *data, size_t size)
{
    return Var{data, size};
}

class Socket
{
public:
    Socket();

    void bind(std::uint16_t port);
    void listen();
    Socket accept();
    void connect(const char *hostname, std::uint16_t port);
    void write(const char *data, size_t size);
    void read(char *data, size_t size);
    void send();
    void receive();
    void close();

    const Fd &fd() const { return m_fd; }
    operator bool() { return !m_fd.closed(); }

    template <typename T>
    Socket &operator<<(Raw<T> data)
    {
        write(data.data(), sizeof(T));
        return *this;
    }

    Socket &operator<<(Var data)
    {
        *this << data.size();
        write(data.data(), data.size());
        return *this;
    }

    template <typename T>
    std::enable_if_t<std::is_integral<T>::value, Socket &> operator<<(T data)
    {
        return *this << raw(data);
    }

    Socket &operator<<(const std::string &data)
    {
        return *this << var(data.data(), data.size());
    }

    Socket &operator<<(const char *data)
    {
        return *this << var(data, std::strlen(data));
    }

    template <typename T>
    Socket &operator>>(Raw<T> data)
    {
        read(data.data(), sizeof(T));
        return *this;
    }

    Socket &operator>>(Var data)
    {
        size_t size;
        if (*this >> size)
        {
            if (data.size() < size)
                throw std::overflow_error{"buffer overflow"};

            read(data.data(), size);
        }
        return *this;
    }

    template <typename T>
    std::enable_if_t<std::is_integral<T>::value, Socket &> operator>>(T &data)
    {
        return *this >> raw(data);
    }

    Socket &operator>>(std::vector<char> &data)
    {
        size_t size;
        if (*this >> size)
        {
            if (size > 4096)
                std::cout << "WARNING: large size " << size << '\n';

            data.resize(size);
            read(data.data(), size);
        }
        return *this;
    }

    Socket &operator>>(std::string &data)
    {
        size_t size;
        if (*this >> size)
        {
            if (size > 4096)
                std::cout << "WARNING: large size " << size << '\n';

            data.resize(size);
            read(&data[0], size);
        }
        return *this;
    }

private:
    Fd m_fd;
    std::ostringstream m_out_buf;
    std::stringstream m_in_buf;
    size_t m_out_size = 0;
    size_t m_in_size = 0;

    explicit Socket(Fd fd);
};

#endif
