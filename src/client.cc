#include "lib.h"
#include <array>
#include <cstring>
#include <string>

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, const char *argv[])
{
    const char *hostname = argc > 1 ? argv[1] : "localhost";
    std::uint16_t port =
        argc > 2 ? static_cast<std::uint16_t>(std::stoi(argv[2])) : 8080;

    int ret;

    // create socket
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    check(sock_fd, "socket");

    // get host info
    hostent *server = gethostbyname(hostname);
    check(server, "gethostbyname");

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    std::memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    // connect to server
    ret = connect(sock_fd, reinterpret_cast<sockaddr *>(&server_addr),
                  sizeof(server_addr));
    check(ret, "connect");

    // send message
    std::string str = "message from client";
    ssize_t bytes_written = write(sock_fd, str.data(), str.size());
    check(bytes_written, "write");

    // receive reply
    std::array<char, 256> buffer;
    ssize_t bytes_read = read(sock_fd, buffer.data(), buffer.size() - 1);
    check(bytes_read, "read");

    buffer[bytes_read] = '\0';
    std::cout << buffer.data() << '\n';

    ret = close(sock_fd);
    check(ret, "close");
}
