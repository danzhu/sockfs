#ifndef SOCK_BUFFER_H
#define SOCK_BUFFER_H

#include <cstddef>

class Buffer
{
public:
    Buffer();
    ~Buffer();
    Buffer(const Buffer &other) = delete;
    Buffer(Buffer &&other);
    Buffer &operator=(const Buffer &other) = delete;
    Buffer &operator=(Buffer &&other);

    void swap(Buffer &other);
    void write(const char *data, std::size_t size);
    char *write(std::size_t size);
    void read(char *data, std::size_t size);
    const char *read(std::size_t size);
    void clear();

    const char *data() const { return m_buffer + m_start; }
    char *data() { return m_buffer + m_start; }
    std::size_t size() const { return m_end - m_start; }
    std::size_t capacity() const { return m_capacity; }

private:
    char *m_buffer = nullptr;
    std::size_t m_start = 0;
    std::size_t m_end = 0;
    std::size_t m_capacity = 0;
};

#endif
