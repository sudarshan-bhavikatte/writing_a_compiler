#pragma once

#include <cstdlib>   // for malloc/free
#include <cstddef>   // for std::byte

class ArenaAllocator {
public:
    explicit ArenaAllocator(size_t bytes)
        : m_size(bytes)
    {
        m_buffer = reinterpret_cast<std::byte*>(malloc(m_size));
        m_offset = m_buffer;
    }

    template<typename T>
    T* alloc() {
        // Allocate memory for T from the arena.
        T* ret = reinterpret_cast<T*>(m_offset);
        m_offset += sizeof(T);
        return ret;
    }

    // Delete the assignment operator
    ArenaAllocator& operator=(const ArenaAllocator& other) = delete;

    ~ArenaAllocator() {
        free(m_buffer);
    }
private:
    size_t m_size;
    std::byte* m_buffer;
    std::byte* m_offset;
};
