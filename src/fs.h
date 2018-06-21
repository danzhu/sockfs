#ifndef SOCK_FS_H
#define SOCK_FS_H

#include "socket.h"

class Fs
{
public:
    explicit Fs(Socket client);

    void run();

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

private:
    Socket m_client;

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

#endif
