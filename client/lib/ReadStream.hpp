#pragma once

#include <type_traits>
#include <vector>

class ReadStream
{
    using Buffer = std::vector<char>;

public:
    ReadStream(Buffer&& _buffer)
        : m_buffer(std::move(_buffer))
    {

    }

    template<typename T>
    ReadStream& operator >> (T& _value)
    {
        static_assert(std::is_integral<T>::value || std::is_floating_point<T>::value, "Unsupported value type");

        std::memcpy(&_value, m_buffer.data() + m_offset, sizeof(_value));
        m_offset += sizeof(_value);

        return *this;
    }

private:
    Buffer::size_type m_offset{ 0 };
    Buffer m_buffer;
};