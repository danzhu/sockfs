#include "console.h"

#include "epoll.h"

Console::Console(Epoll &epoll) : m_epoll{&epoll}, m_fd{0} {}

Console::~Console() {}

void Console::on_read()
{
    // TODO: handle stdin commands
    m_epoll->running(false);
}
