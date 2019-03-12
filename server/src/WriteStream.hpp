#pragma once

#include <vector>

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
		static_assert(false, "Unsupported value type");
	}

	template<>
	WriteStream& operator << (std::int32_t _value)
	{
	}

	const std::vector<char>& getBuffer() const
	{
		return m_buffer;
	}

private:
	std::vector<char> m_buffer;
};