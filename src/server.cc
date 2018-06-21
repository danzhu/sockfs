#include "config.h"
#include "fs.h"
#include "socket.h"

int main(int argc, const char *argv[])
{
    // command-line args
    std::uint16_t port =
        argc > 1 ? static_cast<std::uint16_t>(std::stoi(argv[1])) : PORT;

    [=]() {
        // create socket
        Socket socket;
        socket.bind(port);
        socket.listen();

        return Fs{socket.accept()};
    }().run();
}
