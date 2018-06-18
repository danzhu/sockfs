#ifndef SOCKET_LIB_H
#define SOCKET_LIB_H

#include <iostream>

inline void check(bool cond, const char *msg)
{
    std::cout << "> " << msg;
    if (cond)
    {
        std::cout << '\n';
        return;
    }

    std::cerr << " FAILED\n";
    std::exit(2);
}

inline void check(std::int32_t ret, const char *msg) { return check(ret >= 0, msg); }

inline void check(std::int64_t ret, const char *msg) { return check(ret >= 0, msg); }

inline void check(void *ptr, const char *msg)
{
    return check(ptr != nullptr, msg);
}

#endif
