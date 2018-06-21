#include "sock.h"

namespace
{
template <typename T>
std::enable_if_t<std::is_integral<T>::value, bool> cont(T ret)
{
    std::cout << "  ret " << ret << '\n';
    return ret >= 0;
}
}

Sock::Sock(Socket socket) : m_socket{std::move(socket)} {}

int Sock::getattr(const char *path, struct stat *stbuf)
{
    std::cout << "getattr " << path << '\n';

    m_socket << "getattr" << path;

    int ret;
    m_socket >> ret;
    if (cont(ret))
        m_socket >> raw(*stbuf);

    return ret;
}

int Sock::access(const char *path, int mask)
{
    std::cout << "access " << path << ' ' << mask << '\n';

    m_socket << "access" << path << mask;

    int ret;
    m_socket >> ret;
    cont(ret);

    return ret;
}

int Sock::readlink(const char *path, char *buf, size_t size)
{
    std::cout << "readlink " << path << ' ' << size << '\n';

    m_socket << "readlink" << path << size;

    ssize_t bytes;
    m_socket >> bytes;
    if (cont(bytes))
        m_socket >> var(buf, size);

    return 0;
}

int Sock::readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                  off_t offset, fuse_file_info *fi)
{
    std::cout << "readdir " << path << '\n';

    (void)offset;
    (void)fi;

    m_socket << "readdir" << path;

    int ret;
    m_socket >> ret;
    if (cont(ret))
    {
        bool has_next;
        bool full = false;
        while (m_socket >> has_next, has_next)
        {
            std::string name;
            ino_t ino;
            mode_t mode;
            m_socket >> name >> ino >> mode;

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

    m_socket << "mknod" << path << mode << rdev;

    int ret;
    m_socket >> ret;
    cont(ret);

    return ret;
}

int Sock::mkdir(const char *path, mode_t mode)
{
    std::cout << "mkdir " << path << ' ' << mode << '\n';

    m_socket << "mkdir" << path << mode;

    int ret;
    m_socket >> ret;
    cont(ret);

    return ret;
}

int Sock::unlink(const char *path)
{
    std::cout << "unlink " << path << '\n';

    m_socket << "unlink" << path;

    int ret;
    m_socket >> ret;
    cont(ret);

    return ret;
}

int Sock::rmdir(const char *path)
{
    std::cout << "rmdir " << path << '\n';

    m_socket << "rmdir" << path;

    int ret;
    m_socket >> ret;
    cont(ret);

    return ret;
}

int Sock::symlink(const char *from, const char *to)
{
    std::cout << "symlink " << from << ' ' << to << '\n';

    m_socket << "symlink" << from << to;

    int ret;
    m_socket >> ret;
    cont(ret);

    return ret;
}

int Sock::rename(const char *from, const char *to)
{
    std::cout << "rename " << from << ' ' << to << '\n';

    m_socket << "rename" << from << to;

    int ret;
    m_socket >> ret;
    cont(ret);

    return ret;
}

int Sock::link(const char *from, const char *to)
{
    std::cout << "link " << from << ' ' << to << '\n';

    m_socket << "link" << from << to;

    int ret;
    m_socket >> ret;
    cont(ret);

    return ret;
}

int Sock::chmod(const char *path, mode_t mode)
{
    std::cout << "chmod " << path << ' ' << mode << '\n';

    m_socket << "chmod" << path << mode;

    int ret;
    m_socket >> ret;
    cont(ret);

    return ret;
}

int Sock::chown(const char *path, uid_t uid, gid_t gid)
{
    std::cout << "chown " << path << ' ' << uid << ' ' << gid << '\n';

    m_socket << "chown" << path << uid << gid;

    int ret;
    m_socket >> ret;
    cont(ret);

    return ret;
}

int Sock::truncate(const char *path, off_t size)
{
    std::cout << "truncate " << path << ' ' << size << '\n';

    m_socket << "truncate" << path << size;

    int ret;
    m_socket >> ret;
    cont(ret);

    return ret;
}

int Sock::ftruncate(const char *path, off_t size, fuse_file_info *fi)
{
    std::cout << "ftruncate " << path << ' ' << fi->fh << ' ' << size << '\n';

    m_socket << "ftruncate" << path << static_cast<int>(fi->fh) << size;

    int ret;
    m_socket >> ret;
    cont(ret);

    return ret;
}

int Sock::utimens(const char *path, const timespec ts[2])
{
    std::cout << "utimens " << path << '\n';

    m_socket << "utimens" << path << raw(ts[0]) << raw(ts[1]);

    int ret;
    m_socket >> ret;
    cont(ret);

    return ret;
}

int Sock::create(const char *path, mode_t mode, fuse_file_info *fi)
{
    std::cout << "create " << path << ' ' << mode << ' ' << fi->flags << '\n';

    m_socket << "create" << path << mode << fi->flags;

    int fh;
    m_socket >> fh;
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

    m_socket << "open" << path << fi->flags;

    int fh;
    m_socket >> fh;
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

    m_socket << "read" << path << static_cast<int>(fi->fh) << size << offset;

    ssize_t bytes;
    m_socket >> bytes;
    if (cont(bytes))
        m_socket >> var(buf, size);

    return static_cast<int>(bytes);
}

int Sock::write(const char *path, const char *buf, size_t size, off_t offset,
                fuse_file_info *fi)
{
    std::cout << "write " << path << ' ' << fi->fh << ' ' << size << ' '
              << offset << '\n';

    m_socket << "write" << path << static_cast<int>(fi->fh) << offset
             << var(buf, size);

    ssize_t bytes;
    m_socket >> bytes;
    cont(bytes);

    return static_cast<int>(bytes);
}

int Sock::statfs(const char *path, struct statvfs *stbuf)
{
    std::cout << "statfs " << path << '\n';

    m_socket << "statfs" << path;

    int ret;
    m_socket >> ret;
    if (cont(ret))
        m_socket >> raw(*stbuf);

    return 0;
}

int Sock::release(const char *path, fuse_file_info *fi)
{
    std::cout << "release " << fi->fh << '\n';

    (void)path;

    m_socket << "release" << static_cast<int>(fi->fh);

    int ret;
    m_socket >> ret;
    cont(ret);

    return 0;
}
