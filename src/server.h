#ifndef SOCK_SERVER_H
#define SOCK_SERVER_H

#include "connection.h"
#include "console.h"
#include "fs.h"
#include "socket.h"
#include <unordered_set>

class Epoll;

class Server : public Connection
{
public:
    Server(Epoll &epoll, std::uint16_t port);
    ~Server() = default;

    void on_read() override;

    const Fd &fd() const override { return m_socket.fd(); }

private:
    Epoll *m_epoll;
    Socket m_socket;
    std::unordered_set<Fs> m_connections;
};

#endif
