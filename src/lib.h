#ifndef SOCK_LIB_H
#define SOCK_LIB_H

#include <sstream>
#include <stdexcept>
#include <unistd.h>

inline void check(std::int64_t ret, const char *msg)
{
    if (ret >= 0)
        return;

    std::ostringstream ss;
    ss << msg << " returns " << ret << ", errno " << errno << "\n";
    throw std::runtime_error{ss.str()};
}

inline void check(void *ptr, const char *msg)
{
    if (ptr != nullptr)
        return;

    std::ostringstream ss;
    ss << msg << " returns nullptr\n";
    throw std::runtime_error{ss.str()};
}

#endif
