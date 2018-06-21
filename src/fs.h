#ifndef SOCK_FS_H
#define SOCK_FS_H

#include "connection.h"
#include "socket.h"

class Fs : public Connection
{
public:
    explicit Fs(Socket client);

    void on_read() override;

    const Fd &fd() const override { return m_client.fd(); }

    bool operator==(const Fs &other) const
    {
        return m_client.fd() == other.m_client.fd();
    }

private:
    Socket m_client;

    void getattr();
    void access();
    void readlink();
    void readdir();
    void mknod();
    void mkdir();
    void unlink();
    void rmdir();
    void symlink();
    void rename();
    void link();
    void chmod();
    void chown();
    void truncate();
    void ftruncate();
    void utimens();
    void create();
    void open();
    void read();
    void write();
    void statfs();
    void release();

    template <typename T>
    std::enable_if_t<std::is_integral<T>::value, bool> ret_status(bool cond,
                                                                  T val)
    {
        T ret = cond ? val : -errno;
        std::cout << "  ret " << ret << '\n';
        m_client << ret;
        return cond;
    }

    template <typename T>
    std::enable_if_t<std::is_integral<T>::value, bool> ret_status(T val)
    {
        return ret_status<T>(val >= 0, val);
    }

    template <typename T>
    bool ret_status(const T *ptr)
    {
        return ret_status<int>(ptr != nullptr, 0);
    }

    std::string get_path(const std::string &path);
};

namespace std
{
template <>
class hash<Fs>
{
public:
    std::size_t operator()(const Fs &fs) const
    {
        return std::hash<Fd>{}(fs.fd());
    }
};
}

#endif
