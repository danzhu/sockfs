#include "fs.h"

#include <array>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <unistd.h>

Fs::Fs(Socket client) : m_client{std::move(client)}, m_coder{m_buffer} {}

void Fs::on_read()
{
    // TODO: handle partial message
    coder(m_client.fd()) >> m_buffer;

    std::string op;
    m_coder >> op;

    std::cout << op;

    if (op == "getattr")
        getattr();
    else if (op == "access")
        access();
    else if (op == "readlink")
        readlink();
    else if (op == "readdir")
        readdir();
    else if (op == "mknod")
        mknod();
    else if (op == "mkdir")
        mkdir();
    else if (op == "unlink")
        unlink();
    else if (op == "rmdir")
        rmdir();
    else if (op == "symlink")
        symlink();
    else if (op == "rename")
        rename();
    else if (op == "link")
        link();
    else if (op == "chmod")
        chmod();
    else if (op == "chown")
        chown();
    else if (op == "truncate")
        truncate();
    else if (op == "ftruncate")
        ftruncate();
    else if (op == "utimens")
        utimens();
    else if (op == "create")
        create();
    else if (op == "open")
        open();
    else if (op == "read")
        read();
    else if (op == "write")
        write();
    else if (op == "statfs")
        statfs();
    else if (op == "release")
        release();
    else
    {
        std::cout << " - unsupported operation\n";

        m_coder << EINVAL;
    }

    coder(m_client.fd()) << m_buffer;
}

void Fs::getattr()
{
    std::string path;

    m_coder >> path;

    std::cout << ' ' << path << '\n';

    struct stat stbuf;
    int ret = ::lstat(get_path(path).data(), &stbuf);

    if (ret_status(ret))
        m_coder << raw(stbuf);
}

void Fs::access()
{
    std::string path;
    int mask;

    m_coder >> path >> mask;

    std::cout << ' ' << path << ' ' << mask << '\n';

    int ret = ::access(get_path(path).data(), mask);

    ret_status(ret);
}

void Fs::readlink()
{
    std::string path;
    size_t size;

    m_coder >> path >> size;

    std::cout << ' ' << path << ' ' << size << '\n';

    Buffer buf;
    ssize_t bytes = ::readlink(get_path(path).data(), buf.write(size), size);

    if (ret_status(bytes))
    {
        buf.resize(bytes);
        m_coder << buf;
    }
}

void Fs::readdir()
{
    std::string path;

    m_coder >> path;

    std::cout << ' ' << path << '\n';

    auto dir = ::opendir(get_path(path).data());
    if (!ret_status(dir))
        return;

    dirent *ent;
    while ((ent = ::readdir(dir)) != nullptr)
    {
        ino_t ino = ent->d_ino;
        mode_t mode = ent->d_type << 12;

        // here 'true' indicates that there's still data
        m_coder << true << ent->d_name << ino << mode;
    }

    m_coder << false;

    ::closedir(dir);
}

void Fs::mknod()
{
    std::string path;
    mode_t mode;
    dev_t rdev;

    m_coder >> path >> mode >> rdev;

    std::cout << ' ' << path << ' ' << mode << ' ' << rdev << '\n';

    int ret = ::mknod(get_path(path).data(), mode, rdev);

    ret_status(ret);
}

void Fs::mkdir()
{
    std::string path;
    mode_t mode;

    m_coder >> path >> mode;

    std::cout << ' ' << path << ' ' << mode << '\n';

    int ret = ::mkdir(get_path(path).data(), mode);

    ret_status(ret);
}

void Fs::unlink()
{
    std::string path;

    m_coder >> path;

    std::cout << ' ' << path << '\n';

    int ret = ::unlink(get_path(path).data());

    ret_status(ret);
}

void Fs::rmdir()
{
    std::string path;

    m_coder >> path;

    std::cout << ' ' << path << '\n';

    int ret = ::rmdir(get_path(path).data());

    ret_status(ret);
}

void Fs::symlink()
{
    std::string from;
    std::string to;

    m_coder >> from >> to;

    std::cout << ' ' << from << ' ' << to << '\n';

    int ret = ::symlink(from.data(), get_path(to).data());

    ret_status(ret);
}

void Fs::rename()
{
    std::string from;
    std::string to;

    m_coder >> from >> to;

    std::cout << ' ' << from << ' ' << to << '\n';

    int ret = ::rename(get_path(from).data(), get_path(to).data());

    ret_status(ret);
}

void Fs::link()
{
    std::string from;
    std::string to;

    m_coder >> from >> to;

    std::cout << ' ' << from << ' ' << to << '\n';

    int ret = ::link(get_path(from).data(), get_path(to).data());

    ret_status(ret);
}

void Fs::chmod()
{
    std::string path;
    mode_t mode;

    m_coder >> path >> mode;

    std::cout << ' ' << path << ' ' << mode << '\n';

    int ret = ::chmod(get_path(path).data(), mode);

    ret_status(ret);
}

void Fs::chown()
{
    std::string path;
    uid_t uid;
    gid_t gid;

    m_coder >> path >> uid >> gid;

    std::cout << ' ' << path << ' ' << uid << ' ' << gid << '\n';

    int ret = ::chown(get_path(path).data(), uid, gid);

    ret_status(ret);
}

void Fs::truncate()
{
    std::string path;
    off_t size;

    m_coder >> path >> size;

    std::cout << ' ' << path << ' ' << size << '\n';

    int ret = ::truncate(get_path(path).data(), size);

    ret_status(ret);
}

void Fs::ftruncate()
{
    std::string path;
    int fd;
    off_t size;

    m_coder >> path >> fd >> size;

    std::cout << ' ' << path << ' ' << fd << ' ' << size << '\n';

    int ret = ::ftruncate(fd, size);

    ret_status(ret);
}

void Fs::utimens()
{
    std::string path;
    std::array<timespec, 2> ts;

    m_coder >> path >> raw(ts[0]) >> raw(ts[1]);

    std::cout << ' ' << path << '\n';

    int ret = ::utimensat(AT_FDCWD, get_path(path).data(), ts.data(),
                          AT_SYMLINK_NOFOLLOW);

    ret_status(ret);
}

void Fs::create()
{
    std::string path;
    mode_t mode;
    int flags;

    m_coder >> path >> mode >> flags;

    std::cout << ' ' << path << ' ' << mode << ' ' << flags << '\n';

    int handle = ::open(get_path(path).data(), flags, mode);

    ret_status(handle);
}

void Fs::open()
{
    std::string path;
    int flags;

    m_coder >> path >> flags;

    std::cout << ' ' << path << ' ' << flags << '\n';

    int handle = ::open(get_path(path).data(), flags);

    ret_status(handle);
}

void Fs::read()
{
    std::string path;
    int fd;
    size_t size;
    off_t offset;

    m_coder >> path >> fd >> size >> offset;

    std::cout << ' ' << path << ' ' << fd << ' ' << size << ' ' << offset
              << '\n';

    Buffer buf;
    ssize_t bytes = ::pread(fd, buf.write(size), size, offset);

    if (ret_status(bytes))
    {
        buf.resize(bytes);
        m_coder << buf;
    }
}

void Fs::write()
{
    std::string path;
    int fd;
    off_t offset;
    Buffer buf;

    m_coder >> path >> fd >> offset >> buf;

    std::cout << ' ' << path << ' ' << fd << ' ' << buf.size() << ' ' << offset
              << '\n';

    ssize_t bytes = ::pwrite(fd, buf.data(), buf.size(), offset);

    ret_status(bytes);
}

void Fs::statfs()
{
    std::string path;

    m_coder >> path;

    std::cout << ' ' << path << '\n';

    struct statvfs stbuf;
    int ret = ::statvfs(get_path(path).data(), &stbuf);

    if (ret_status(ret))
        m_coder << raw(stbuf);
}

void Fs::release()
{
    int fd;

    m_coder >> fd;

    std::cout << ' ' << fd << '\n';

    auto ret = ::close(fd);

    ret_status(ret);
}

std::string Fs::get_path(const std::string &path) { return '.' + path; }
