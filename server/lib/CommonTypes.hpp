#pragma once

#include <cstdint>
#include <type_traits>

namespace common
{
    using ID = std::int32_t;
    
    static constexpr ID InvalidID{0};
    
    enum class PacketType : std::int32_t
    {
        Undefined = 0,
        InitRequest = 2,
        InitResponse = 3,
        Frame = 4
    };

    enum class Slot : std::int32_t
    {
        Undefined = 0,
        Top = 1,
        Bottom = 2,
        Left = 3,
        Right = 4
    };

    enum class Direction : std::int32_t
    {
        Undefined = 0,
        Left = 1,
        Right = 2,
        Up = 3,
        Down = 4
    };
    
    struct ClientFrame
    {
        const ID id{InvalidID};
        Vec2 dir;
        
		ClientFrame(ID _id)
			: id(_id)
		{

		}

        bool operator < (const ClientFrame& _frame) const
        {
            return id < _frame.id;
        }
    };

    struct ServerFrame
    {
        const ID id{InvalidID};
        Vec2 pos;
        
		ServerFrame(ID _id)
			: id(_id)
		{

		}

        bool operator < (const ServerFrame& _frame) const
        {
            return id < _frame.id;
        }
    };
    
    struct Snapshot
    {
        const ID id{InvalidID};
        std::map<ID, std::set<ServerFrame>> frames;
        
		Snapshot(ID _id)
			: id(_id)
		{

		}

        bool operator < (const Snapshot& _snapshot) const
        {
            return id < _snapshot.id;
        }
    };

	namespace utils
	{
		static bool IsDirectionValidForSlot(common::Direction _direction, common::Slot _slot)
		{
			switch (_slot)
			{
			case Slot::Top:
				[[fallthrough]];
			case Slot::Bottom:
				return _direction == Direction::Left || _direction == Direction::Right;

			case Slot::Left:
				[[fallthrough]];
			case Slot::Right:
				return _direction == Direction::Up || _direction == Direction::Down;

			default:
				break;
			}

			return false;
		}

		static Vec2 GetVectorFromDirection(Direction _direction)
		{
			switch (_direction)
			{
			case Direction::Left: return Vec2{ -1.0f, 0.0f };
			case Direction::Right: return Vec2{ 1.0f, 0.0f };
			case Direction::Up: return Vec2{ 0.0f, 1.0f };
			case Direction::Down: return Vec2{ 0.0f, -1.0f };

			default:
				break;
			}

			return Vec2{ 0.0f, 0.0f };
		}
	}
}

template<typename Source>
constexpr auto static_enum_cast(Source _value) -> typename std::underlying_type<Source>::type
{
    return static_cast<typename std::underlying_type<Source>::type>(_value);
}