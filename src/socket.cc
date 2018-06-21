#include "socket.h"

#include "lib.h"
#include <algorithm>
#include <array>
#include <cassert>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

Socket::Socket() : Socket{socket(AF_INET, SOCK_STREAM, 0)} {}

Socket::Socket(int fd) : m_fd{fd}
{
    check(fd, "open socket");

    m_in_buf.exceptions(std::stringstream::failbit);

    std::cerr << "> open\n";
}

Socket::~Socket()
{
    if (!m_closed)
        close();
}

Socket::Socket(Socket &&other)
    : m_fd{other.m_fd}, m_closed{other.m_closed},
      m_out_buf{std::move(other.m_out_buf)},
      m_in_buf{std::move(other.m_in_buf)}, m_out_size{other.m_out_size},
      m_in_size{other.m_in_size}
{
    other.m_closed = true;
}

Socket &Socket::operator=(Socket &&other)
{
    swap(other);
    return *this;
}

void Socket::swap(Socket &other)
{
    std::swap(m_fd, other.m_fd);
    std::swap(m_closed, other.m_closed);
    std::swap(m_out_buf, other.m_out_buf);
    std::swap(m_in_buf, other.m_in_buf);
    std::swap(m_out_size, other.m_out_size);
    std::swap(m_in_size, other.m_in_size);
}

void Socket::bind(std::uint16_t port)
{
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    // port, converted to network byte order
    server_addr.sin_port = htons(port);
    // address set to current machine ip
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // bind socket to address
    int ret = ::bind(m_fd, reinterpret_cast<sockaddr *>(&server_addr),
                     sizeof(server_addr));
    check(ret, "bind");

    std::cerr << "> bound\n";
}

void Socket::listen()
{
    // open socket to listen for connections
    constexpr int BACKLOG_QUEUE_SIZE = 5;
    int ret = ::listen(m_fd, BACKLOG_QUEUE_SIZE);
    // should always succeed
    check(ret, "listen");

    std::cerr << "> listening\n";
}

Socket Socket::accept()
{
    // wait for a connection
    sockaddr_in cli_addr{};
    socklen_t cli_len = sizeof(cli_addr);
    int cli_sock_fd =
        ::accept(m_fd, reinterpret_cast<sockaddr *>(&cli_addr), &cli_len);
    check(cli_sock_fd, "accept");

    std::cerr << "> accepted\n";

    return Socket{cli_sock_fd};
}

void Socket::connect(const char *hostname, std::uint16_t port)
{
    // get host info
    auto server = gethostbyname(hostname);
    check(server, "gethostbyname");

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    std::memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    // connect to server
    auto ret = ::connect(m_fd, reinterpret_cast<sockaddr *>(&server_addr),
                         sizeof(server_addr));
    check(ret, "connect");

    std::cerr << "> connected\n";
}

void Socket::write(const char *data, std::size_t size)
{
    m_out_buf.write(data, size);
    m_out_size += size;
}

void Socket::read(char *data, std::size_t size)
{
    send();

    while (m_in_size < size)
    {
        if (m_closed)
            return;

        receive();
    }

    m_in_buf.read(data, size);
    m_in_size -= size;
}

void Socket::send()
{
    if (m_closed)
        throw std::runtime_error{"closed"};

    if (m_out_size == 0)
        return;

    if (m_in_size > 0)
        std::cerr << "WARNING: " << m_in_size << " bytes unread\n";

    auto msg = m_out_buf.str();
    assert(msg.size() == m_out_size);

    auto bytes_written = ::write(m_fd, msg.data(), msg.size());
    check(bytes_written, "write");

    std::cerr << "> sent " << bytes_written << " bytes\n";

    m_out_buf.str("");
    m_out_size = 0;
}

void Socket::receive()
{
    if (m_closed)
        throw std::runtime_error{"closed"};

    std::array<char, 4096> msg;
    auto bytes_read = ::read(m_fd, msg.data(), msg.size());
    check(bytes_read, "read");

    std::cerr << "> received " << bytes_read << " bytes\n";

    if (bytes_read == 0)
    {
        close();
        return;
    }

    m_in_buf.write(msg.data(), bytes_read);
    m_in_size += bytes_read;
}

void Socket::close()
{
    if (m_closed)
        throw std::runtime_error{"closed"};

    send();

    m_closed = true;
    int ret = ::close(m_fd);
    check(ret, "close socket");

    std::cerr << "> closed\n";
}
