#pragma once

#include <vector>
#include <iterator>
#include <type_traits>

class WriteStream
{
public:
    WriteStream(int _capacity)
    {
        m_buffer.reserve(_capacity);
    }

    template<typename T>
    WriteStream& operator << (T _value)
    {
        static_assert(std::is_integral<T>::value || std::is_floating_point<T>::value, "Unsupported value type");
        
        auto begin = reinterpret_cast<char*>(&_value);
        auto end = reinterpret_cast<char*>(&_value) + sizeof(_value);

        m_buffer.insert(m_buffer.end(), begin, end);
        return *this;
    }

    const std::vector<char>& getBuffer() const
    {
        return m_buffer;
    }

private:
    std::vector<char> m_buffer;
};
