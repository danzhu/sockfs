#include "buffer.h"

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <utility>

Buffer::Buffer(std::size_t size)
{
    if (size == 0)
        return;

    m_capacity = 16;
    while (m_capacity < size)
        m_capacity *= 2;

    m_buffer = static_cast<char *>(std::malloc(m_capacity));
    m_end = size;
}

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
    resize(this->size() + size);
    return m_buffer + m_end - size;
}

void Buffer::read(char *data, std::size_t size)
{
    std::memcpy(data, read(size), size);
}

const char *Buffer::read(std::size_t size)
{
    if (size > this->size())
        throw std::out_of_range{"buffer underflow"};

    m_start += size;
    return m_buffer + m_start - size;
}

void Buffer::resize(std::size_t size)
{
    // move to start if we are empty to reduce reallocations
    if (empty())
        m_start = m_end = 0;

    auto new_end = m_start + size;
    if (new_end <= m_capacity)
    {
        // we have enough space

        m_end = new_end;
    }
    else if (size <= m_capacity)
    {
        // enough space if we move data to the front

        std::memmove(m_buffer, data(), this->size());

        m_start = 0;
        m_end = size;
    }
    else
    {
        // need to reallocate

        Buffer new_buf{size};
        if (this->size() > 0)
            std::memcpy(new_buf.data(), data(), this->size());

        *this = std::move(new_buf);
    }

    assert(this->size() == size);
}

void Buffer::clear() { *this = Buffer{}; }
