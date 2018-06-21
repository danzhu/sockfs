#ifndef SOCK_SOCK_H
#define SOCK_SOCK_H

#include "buffer.h"
#include "coder.h"
#include "socket.h"
#include <fuse.h>

class Sock
{
public:
    explicit Sock(Socket socket);

    int getattr(const char *path, struct stat *stbuf);
    int access(const char *path, int mask);
    int readlink(const char *path, char *buf, size_t size);
    int readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                off_t offset, fuse_file_info *fi);
    int mknod(const char *path, mode_t mode, dev_t rdev);
    int mkdir(const char *path, mode_t mode);
    int unlink(const char *path);
    int rmdir(const char *path);
    int symlink(const char *from, const char *to);
    int rename(const char *from, const char *to);
    int link(const char *from, const char *to);
    int chmod(const char *path, mode_t mode);
    int chown(const char *path, uid_t uid, gid_t gid);
    int truncate(const char *path, off_t size);
    int ftruncate(const char *path, off_t size, fuse_file_info *fi);
    int utimens(const char *path, const timespec ts[2]);
    int create(const char *path, mode_t mode, fuse_file_info *fi);
    int open(const char *path, fuse_file_info *fi);
    int read(const char *path, char *buf, size_t size, off_t offset,
             fuse_file_info *fi);
    int write(const char *path, const char *buf, size_t size, off_t offset,
              fuse_file_info *fi);
    int statfs(const char *path, struct statvfs *stbuf);
    int release(const char *path, fuse_file_info *fi);

private:
    Socket m_socket;
    Buffer m_buffer;
    Coder<Buffer> m_coder;

    void exchange();
};

#endif
