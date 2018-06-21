#include "buffer.h"

#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <utility>

Buffer::Buffer() {}

Buffer::~Buffer() { std::free(m_buffer); }

Buffer::Buffer(Buffer &&other) : Buffer{} { swap(other); }

Buffer &Buffer::operator=(Buffer &&other)
{
    swap(other);
    return *this;
}

void Buffer::swap(Buffer &other)
{
    std::swap(m_buffer, other.m_buffer);
    std::swap(m_start, other.m_start);
    std::swap(m_end, other.m_end);
    std::swap(m_capacity, other.m_capacity);
}

void Buffer::write(const char *data, std::size_t size)
{
    std::memcpy(write(size), data, size);
}

char *Buffer::write(std::size_t size)
{
    // move to start if we are empty to reduce reallocations
    if (this->size() == 0)
    {
        m_start = 0;
        m_end = 0;
    }

    auto new_end = m_end + size;
    if (new_end > m_capacity)
    {
        // need to move things around

        auto new_size = new_end - m_start;
        if (new_size > m_capacity)
        {
            // need to reallocate

            auto new_cap = m_capacity == 0 ? 16 : m_capacity;
            while (new_cap < new_size)
                new_cap *= 2;

            auto new_buf = static_cast<char *>(std::malloc(new_cap));
            if (m_buffer != nullptr)
            {
                std::memcpy(new_buf, data(), this->size());
                std::free(m_buffer);
            }
            m_buffer = new_buf;

            m_capacity = new_cap;
        }
        else
        {
            // otherwise, just move data to the front

            std::memmove(m_buffer, data(), this->size());
        }

        m_end -= m_start;
        m_start = 0;
    }

    auto old_data = m_buffer + m_end;
    m_end = new_end;
    return old_data;
}

void Buffer::read(char *data, std::size_t size)
{
    std::memcpy(data, read(size), size);
}

const char *Buffer::read(std::size_t size)
{
    auto new_start = m_start + size;
    if (new_start > m_end)
        throw std::out_of_range{"buffer underflow"};

    auto old_data = m_buffer + m_start;
    m_start = new_start;
    return old_data;
}

void Buffer::clear() { *this = Buffer{}; }
