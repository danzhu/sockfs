#include "config.h"
#include "lib.h"
#include "sock.h"
#include <cassert>
#include <iostream>
#include <memory>
#include <unistd.h>

namespace
{
void show_help(const char *prog)
{
    std::cout << "usage: " << prog << " mountpoint [options] \n"
              << '\n'
              << "general options:\n"
              << "    -o opt,[opt...]        mount options\n"
              << "    -h   --help            print help\n"
              << "    -V   --version         print version\n"
              << '\n'
              << "sockfs options:\n"
              << "    --host=HOST            specify host (default: " << HOST
              << ")\n"
              << "    --port=PORT            specify port (default: " << PORT
              << ")\n"
              << '\n';
}

Sock &get_sock()
{
    return *static_cast<Sock *>(fuse_get_context()->private_data);
}
}

int main(int argc, const char *argv[])
{
    struct Options
    {
        const char *host = HOST;
        int port = PORT;
        int show_help;
        int show_version;
    } opt{};

    fuse_opt option_spec[] = {{"--host=%s", offsetof(Options, host), 1},
                              {"--port=%d", offsetof(Options, port), 1},
                              {"--help", offsetof(Options, show_help), 1},
                              {"--version", offsetof(Options, show_version), 1},
                              FUSE_OPT_END};

    fuse_operations ops{};
    ops.getattr = [](auto... args) { return get_sock().getattr(args...); };
    ops.access = [](auto... args) { return get_sock().access(args...); };
    ops.readlink = [](auto... args) { return get_sock().readlink(args...); };
    ops.readdir = [](auto... args) { return get_sock().readdir(args...); };
    ops.mknod = [](auto... args) { return get_sock().mknod(args...); };
    ops.mkdir = [](auto... args) { return get_sock().mkdir(args...); };
    ops.unlink = [](auto... args) { return get_sock().unlink(args...); };
    ops.rmdir = [](auto... args) { return get_sock().rmdir(args...); };
    ops.symlink = [](auto... args) { return get_sock().symlink(args...); };
    ops.rename = [](auto... args) { return get_sock().rename(args...); };
    ops.link = [](auto... args) { return get_sock().link(args...); };
    ops.chmod = [](auto... args) { return get_sock().chmod(args...); };
    ops.chown = [](auto... args) { return get_sock().chown(args...); };
    ops.truncate = [](auto... args) { return get_sock().truncate(args...); };
    ops.ftruncate = [](auto... args) { return get_sock().ftruncate(args...); };
    ops.utimens = [](auto... args) { return get_sock().utimens(args...); };
    ops.create = [](auto... args) { return get_sock().create(args...); };
    ops.open = [](auto... args) { return get_sock().open(args...); };
    ops.read = [](auto... args) { return get_sock().read(args...); };
    ops.write = [](auto... args) { return get_sock().write(args...); };
    ops.statfs = [](auto... args) { return get_sock().statfs(args...); };
    ops.release = [](auto... args) { return get_sock().release(args...); };

    fuse_args args = FUSE_ARGS_INIT(argc, const_cast<char **>(argv));

    int ret = fuse_opt_parse(&args, &opt, option_spec, nullptr);
    if (ret < 0)
        return 1;

    std::unique_ptr<Sock> sock;

    if (opt.show_help)
    {
        show_help(args.argv[0]);
        assert(fuse_opt_add_arg(&args, "-ho") == 0);
        args.argv[0] = const_cast<char *>("");
    }
    else if (opt.show_version)
    {
        std::cout << "sockfs version: 0.1\n";
        assert(fuse_opt_add_arg(&args, "--version") == 0);
    }
    else
    {
        Socket socket;
        socket.connect(opt.host, static_cast<std::uint16_t>(opt.port));

        sock = std::make_unique<Sock>(std::move(socket));
    }

    ret = fuse_main(args.argc, args.argv, &ops, sock.get());

    fuse_opt_free_args(&args);

    return ret;
}
