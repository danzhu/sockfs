#ifndef SOCK_SOCKET_H
#define SOCK_SOCKET_H

#include "fd.h"

class Socket
{
public:
    Socket();

    void bind(std::uint16_t port);
    void listen();
    Socket accept();
    void connect(const char *hostname, std::uint16_t port);

    const Fd &fd() const { return m_fd; }
    Fd &fd() { return m_fd; }
    operator bool() { return !m_fd.closed(); }

private:
    Fd m_fd;

    explicit Socket(Fd fd);
};

#endif
