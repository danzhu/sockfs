#ifndef SOCK_EPOLL_H
#define SOCK_EPOLL_H

#include "fd.h"

class Connection;

class Epoll
{
public:
    Epoll();

    void add(Connection &conn);
    void run();

    bool running() const { return m_running; }
    void running(bool running) { m_running = running; }

private:
    Fd m_fd;
    bool m_running = true;
};

#endif
