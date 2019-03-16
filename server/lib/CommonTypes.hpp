#pragma once

#include <cstdint>
#include <type_traits>

enum class PacketType : std::int32_t
{
    Undefined = 0,
    InitRequest = 2,
    InitResponse = 3,
    Frame = 4
};

template<typename Source>
constexpr auto static_enum_cast(Source _value) -> typename std::underlying_type<Source>::type
{
    return static_cast<typename std::underlying_type<Source>::type>(_value);
}