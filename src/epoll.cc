#include "epoll.h"

#include "config.h"
#include "connection.h"
#include "lib.h"
#include <array>
#include <sys/epoll.h>

Epoll::Epoll() : m_fd{epoll_create1(0)} {}

void Epoll::add(Connection &conn)
{
    epoll_event event{};
    event.events = EPOLLIN;
    event.data.ptr = &conn;

    auto ret = epoll_ctl(m_fd.data(), EPOLL_CTL_ADD, conn.fd().data(), &event);
    check(ret, "epoll_ctl");
}

void Epoll::run()
{
    while (m_running)
    {
        std::array<epoll_event, EVENT_SIZE> events;
        auto count = epoll_wait(m_fd.data(), events.data(), events.size(), -1);

        for (auto i = 0; i < count; ++i)
        {
            auto &event = events[i];
            auto &conn = *static_cast<Connection *>(event.data.ptr);

            switch (event.events)
            {
            case EPOLLIN:
                conn.on_read();
                break;

            case EPOLLERR:
                conn.on_error();
                break;
            }
        }
    }
}
