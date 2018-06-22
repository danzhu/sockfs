#include "sock.h"

#include <iostream>

namespace
{
template <typename T>
std::enable_if_t<std::is_integral<T>::value, bool> cont(T ret)
{
    std::cout << "  ret " << ret << '\n';
    return ret >= 0;
}
}

Sock::Sock(Socket socket) : m_socket{std::move(socket)}, m_coder{m_buffer} {}

int Sock::getattr(const char *path, struct stat *stbuf)
{
    std::cout << "getattr " << path << '\n';

    m_coder << "getattr" << path;

    exchange();

    int ret;
    m_coder >> ret;
    if (cont(ret))
        m_coder >> raw(*stbuf);

    return ret;
}

int Sock::access(const char *path, int mask)
{
    std::cout << "access " << path << ' ' << mask << '\n';

    m_coder << "access" << path << mask;

    exchange();

    int ret;
    m_coder >> ret;
    cont(ret);

    return ret;
}

int Sock::readlink(const char *path, char *buf, size_t size)
{
    std::cout << "readlink " << path << ' ' << size - 1 << '\n';

    m_coder << "readlink" << path << size - 1;

    exchange();

    ssize_t bytes;
    m_coder >> bytes;
    if (cont(bytes))
    {
        m_coder >> var(buf, size);
        buf[bytes] = '\0';
    }

    return 0;
}

int Sock::readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                  off_t offset, fuse_file_info *fi)
{
    std::cout << "readdir " << path << '\n';

    (void)offset;
    (void)fi;

    m_coder << "readdir" << path;

    exchange();

    int ret;
    m_coder >> ret;
    if (cont(ret))
    {
        bool has_next;
        bool full = false;
        while (m_coder >> has_next, has_next)
        {
            std::string name;
            ino_t ino;
            mode_t mode;
            m_coder >> name >> ino >> mode;

            // we still need to read all the data even if we don't need them
            if (!full)
            {
                struct stat st;
                st.st_ino = ino;
                st.st_mode = mode;
                full = filler(buf, name.data(), &st, 0);
            }
        }
    }

    return ret;
}

int Sock::mknod(const char *path, mode_t mode, dev_t rdev)
{
    std::cout << "mknod " << path << ' ' << mode << ' ' << rdev << '\n';

    m_coder << "mknod" << path << mode << rdev;

    exchange();

    int ret;
    m_coder >> ret;
    cont(ret);

    return ret;
}

int Sock::mkdir(const char *path, mode_t mode)
{
    std::cout << "mkdir " << path << ' ' << mode << '\n';

    m_coder << "mkdir" << path << mode;

    exchange();

    int ret;
    m_coder >> ret;
    cont(ret);

    return ret;
}

int Sock::unlink(const char *path)
{
    std::cout << "unlink " << path << '\n';

    m_coder << "unlink" << path;

    exchange();

    int ret;
    m_coder >> ret;
    cont(ret);

    return ret;
}

int Sock::rmdir(const char *path)
{
    std::cout << "rmdir " << path << '\n';

    m_coder << "rmdir" << path;

    exchange();

    int ret;
    m_coder >> ret;
    cont(ret);

    return ret;
}

int Sock::symlink(const char *from, const char *to)
{
    std::cout << "symlink " << from << ' ' << to << '\n';

    m_coder << "symlink" << from << to;

    exchange();

    int ret;
    m_coder >> ret;
    cont(ret);

    return ret;
}

int Sock::rename(const char *from, const char *to)
{
    std::cout << "rename " << from << ' ' << to << '\n';

    m_coder << "rename" << from << to;

    exchange();

    int ret;
    m_coder >> ret;
    cont(ret);

    return ret;
}

int Sock::link(const char *from, const char *to)
{
    std::cout << "link " << from << ' ' << to << '\n';

    m_coder << "link" << from << to;

    exchange();

    int ret;
    m_coder >> ret;
    cont(ret);

    return ret;
}

int Sock::chmod(const char *path, mode_t mode)
{
    std::cout << "chmod " << path << ' ' << mode << '\n';

    m_coder << "chmod" << path << mode;

    exchange();

    int ret;
    m_coder >> ret;
    cont(ret);

    return ret;
}

int Sock::chown(const char *path, uid_t uid, gid_t gid)
{
    std::cout << "chown " << path << ' ' << uid << ' ' << gid << '\n';

    m_coder << "chown" << path << uid << gid;

    exchange();

    int ret;
    m_coder >> ret;
    cont(ret);

    return ret;
}

int Sock::truncate(const char *path, off_t size)
{
    std::cout << "truncate " << path << ' ' << size << '\n';

    m_coder << "truncate" << path << size;

    exchange();

    int ret;
    m_coder >> ret;
    cont(ret);

    return ret;
}

int Sock::ftruncate(const char *path, off_t size, fuse_file_info *fi)
{
    std::cout << "ftruncate " << path << ' ' << fi->fh << ' ' << size << '\n';

    m_coder << "ftruncate" << path << static_cast<int>(fi->fh) << size;

    exchange();

    int ret;
    m_coder >> ret;
    cont(ret);

    return ret;
}

int Sock::utimens(const char *path, const timespec ts[2])
{
    std::cout << "utimens " << path << '\n';

    m_coder << "utimens" << path << raw(ts[0]) << raw(ts[1]);

    exchange();

    int ret;
    m_coder >> ret;
    cont(ret);

    return ret;
}

int Sock::create(const char *path, mode_t mode, fuse_file_info *fi)
{
    std::cout << "create " << path << ' ' << mode << ' ' << fi->flags << '\n';

    m_coder << "create" << path << mode << fi->flags;

    exchange();

    int fh;
    m_coder >> fh;
    if (cont(fh))
    {
        fi->fh = fh;
        return 0;
    }

    return fh;
}

int Sock::open(const char *path, fuse_file_info *fi)
{
    std::cout << "open " << path << ' ' << fi->flags << '\n';

    m_coder << "open" << path << fi->flags;

    exchange();

    int fh;
    m_coder >> fh;
    if (cont(fh))
    {
        fi->fh = fh;
        return 0;
    }

    return fh;
}

int Sock::read(const char *path, char *buf, size_t size, off_t offset,
               fuse_file_info *fi)
{
    std::cout << "read " << path << ' ' << fi->fh << ' ' << size << ' '
              << offset << '\n';

    m_coder << "read" << path << static_cast<int>(fi->fh) << size << offset;

    exchange();

    ssize_t bytes;
    m_coder >> bytes;
    if (cont(bytes))
        m_coder >> var(buf, size);

    return static_cast<int>(bytes);
}

int Sock::write(const char *path, const char *buf, size_t size, off_t offset,
                fuse_file_info *fi)
{
    std::cout << "write " << path << ' ' << fi->fh << ' ' << size << ' '
              << offset << '\n';

    m_coder << "write" << path << static_cast<int>(fi->fh) << offset
            << var(buf, size);

    exchange();

    ssize_t bytes;
    m_coder >> bytes;
    cont(bytes);

    return static_cast<int>(bytes);
}

int Sock::statfs(const char *path, struct statvfs *stbuf)
{
    std::cout << "statfs " << path << '\n';

    m_coder << "statfs" << path;

    exchange();

    int ret;
    m_coder >> ret;
    if (cont(ret))
        m_coder >> raw(*stbuf);

    return 0;
}

int Sock::release(const char *path, fuse_file_info *fi)
{
    std::cout << "release " << fi->fh << '\n';

    (void)path;

    m_coder << "release" << static_cast<int>(fi->fh);

    exchange();

    int ret;
    m_coder >> ret;
    cont(ret);

    return 0;
}

void Sock::exchange() { coder(m_socket.fd()) << m_buffer >> m_buffer; }
