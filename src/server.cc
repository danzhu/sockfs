#include "lib.h"
#include <array>
#include <cassert>
#include <iostream>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, const char *argv[])
{
    std::uint16_t port =
        argc > 2 ? static_cast<std::uint16_t>(std::stoi(argv[2])) : 8080;

    int ret;

    // create socket
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    check(sock_fd, "socket");

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    // port, converted to network byte order
    server_addr.sin_port = htons(port);
    // address set to current machine ip
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // bind socket to address
    ret = bind(sock_fd, reinterpret_cast<sockaddr *>(&server_addr),
               sizeof(server_addr));
    check(ret, "bind");

    // open socket to listen for connections
    constexpr int BACKLOG_QUEUE_SIZE = 5;
    ret = listen(sock_fd, BACKLOG_QUEUE_SIZE);
    // should always succeed
    check(ret, "listen");

    while (true)
    {
        // wait for a connection
        sockaddr_in cli_addr{};
        socklen_t cli_len = sizeof(cli_addr);
        int cli_sock_fd =
            accept(sock_fd, reinterpret_cast<sockaddr *>(&cli_addr), &cli_len);
        check(cli_sock_fd, "accept");

        // read data from client
        std::array<char, 256> buffer;
        ssize_t bytes_read =
            read(cli_sock_fd, buffer.data(), buffer.size() - 1);
        check(bytes_read, "read");

        buffer[bytes_read] = '\0';
        std::cout << buffer.data() << '\n';

        // send reply to client
        std::string message = "message from server";
        ssize_t bytes_written =
            write(cli_sock_fd, message.data(), message.size());
        check(bytes_written, "write");

        ret = close(cli_sock_fd);
        check(ret, "close");
    }

    // ret = close(sock_fd);
    // check(ret, "close");
}
