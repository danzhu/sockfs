#ifndef SOCK_FD_H
#define SOCK_FD_H

#include <functional>
#include <unistd.h>

class Fd
{
public:
    explicit Fd(int fd);
    ~Fd();
    Fd(const Fd &other) = delete;
    Fd(Fd &&other);
    Fd &operator=(const Fd &other) = delete;
    Fd &operator=(Fd &&other);

    bool operator==(const Fd &other) const { return m_data == other.m_data; }

    ssize_t write(const char *data, size_t size);
    ssize_t read(char *data, size_t size);
    void close();
    void release();
    void swap(Fd &other);

    int data() const { return m_data; }
    bool closed() const { return m_data < 0; }

private:
    int m_data;
};

namespace std
{
template <>
class hash<Fd>
{
public:
    std::size_t operator()(const Fd &fd) const
    {
        return std::hash<int>{}(fd.data());
    }
};
}

#endif
