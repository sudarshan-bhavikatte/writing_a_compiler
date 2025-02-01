#pragma once

class ArenaAllocator {
public:
    inline ArenaAllocator(size_t bytes)
    :m_size(bytes)
    {
        m_buffer = static_cast<byte*>(malloc(m_size));
        m_offset = m_buffer;
    }

    template<typename T>;
    inline T* alloc() {
        byte* offset = m_offset;
        offset += sizeof(T);
        return offset;
    }

    inline ArenaAllocator operator = ( const ArenaAllocator& other) = delete;

    inline ~ArenaAllocator() {
        free(m_buffer);
    }
private:
    size_t m_size;
    byte* m_buffer;
    byte* m_offset;
};