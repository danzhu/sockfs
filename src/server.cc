#include "server.h"

#include "config.h"
#include "epoll.h"

Server::Server(Epoll &epoll, std::uint16_t port) : m_epoll{&epoll}
{
    m_socket.bind(port);
    m_socket.listen();
}

void Server::on_read()
{
    auto it = m_connections.emplace(m_socket.accept());
    m_epoll->add(const_cast<Fs &>(*it.first));
}

int main(int argc, const char *argv[])
{
    // command-line args
    std::uint16_t port =
        argc > 1 ? static_cast<std::uint16_t>(std::stoi(argv[1])) : PORT;

    Epoll epoll;

    Server server{epoll, port};
    Console console{epoll};

    epoll.add(server);
    epoll.add(console);

    epoll.run();
}
