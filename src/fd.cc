#include "fd.h"

#include "lib.h"
#include <iostream>
#include <unistd.h>

Fd::Fd(int fd) : m_data{fd} { check(fd, "open"); }

Fd::~Fd()
{
    if (!closed())
        close();
}

Fd::Fd(Fd &&other) : m_data{other.m_data} { other.release(); }

Fd &Fd::operator=(Fd &&other)
{
    swap(other);
    return *this;
}

ssize_t Fd::write(const char *data, std::size_t size)
{
    auto bytes = ::write(m_data, data, size);
    check(bytes, "write");
    std::cerr << "> wrote " << bytes << '\n';
    return bytes;
}

ssize_t Fd::read(char *data, std::size_t size)
{
    auto bytes = ::read(m_data, data, size);
    check(bytes, "read");
    std::cerr << "> read " << bytes << '\n';
    return bytes;
}

void Fd::close()
{
    int ret = ::close(m_data);
    check(ret, "close");

    release();
}

void Fd::release() { m_data = -1; }

void Fd::swap(Fd &other) { std::swap(m_data, other.m_data); }
