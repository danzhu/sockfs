#ifndef SOCK_CONSOLE_H
#define SOCK_CONSOLE_H

#include "connection.h"
#include "fd.h"

class Epoll;

class Console : public Connection
{
public:
    Console(Epoll &epoll);
    ~Console();

    void on_read() override;

    const Fd &fd() const override { return m_fd; }

private:
    Epoll *m_epoll;
    Fd m_fd;
};

#endif
