#pragma once

#include <vector>

class ReadStream
{
public:
	ReadStream(std::vector<char>&& _buffer)
		: m_buffer(std::move(_buffer))
	{

	}

	template<typename T>
	ReadStream& operator >> (T& _value)
	{
		static_assert(false, "Unsupported value type");
	}

	template<>
	ReadStream& operator >> (std::int32_t& _value)
	{
		return *this;
	}

private:
	std::vector<char> m_buffer;
};