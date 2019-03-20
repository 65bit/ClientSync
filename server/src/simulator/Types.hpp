#pragma once

#include <cstdint>
#include <set>
#include <map>
#include <functional>

namespace simulator
{
    using ID = std::int32_t;
    static constexpr ID InvalidID{0};
    
    enum class Slot
    {
        Undefined,
        Top,
        Bottom,
        Left,
        Right
    };
    
    enum class Direction
    {
        Undefined,
        Left,
        Right,
        Up,
        Down
    };
    
    struct Vec2
    {
        float x{0.0f};
        float y{0.0f};
        
        Vec2() = default;
        
        Vec2(float _x, float _y)
        : x(_x)
        , y(_y)
        {
            
        }
        
        Vec2 operator * (float _value) const
        {
            return {x * _value, y * _value};
        }
        
        Vec2 operator + (const Vec2& _vec) const
        {
            return {x + _vec.x, y + _vec.y};
        }
        
        Vec2 operator - (const Vec2& _vec) const
        {
            return {x - _vec.x, y - _vec.y};
        }
    };
    
    struct LoggerProxy
    {
        std::function<void(const std::string&)> info;
        std::function<void(const std::string&)> warning;
        std::function<void(const std::string&)> error;
    };

    struct PlayerConfig
    {
        unsigned maxHealth{0};
        float moveSpeed{0.0f};
        float platformWidth{0.0f};
        float platformHeight{0.0f};
    };
    
    struct GunConfig
    {
        float rotationSpeed{0.0f};
        unsigned shootDelayMin{0};
        unsigned shootDelayMax{0};
        unsigned damage{0};
    };
    
    struct FieldConfig
    {
        float width{0.0f};
        float height{0.0f};
    };
    
    struct Config
    {
        FieldConfig field;
        PlayerConfig player;
        GunConfig gun;
    };
    
    struct PlayerInput
    {
        ID id{InvalidID};
        Direction dir{Direction::Undefined};
        
        PlayerInput(ID _id)
        : id(_id)
        {
            
        }
        
        bool operator < (const PlayerInput& _input) const
        {
            return id < _input.id;
        }
    };
    
    struct PlayerOutput
    {
        ID id{InvalidID};
        Vec2 pos;
        
        PlayerOutput(ID _id)
        : id(_id)
        {
            
        }
        
        bool operator < (const PlayerOutput& _output) const
        {
            return id < _output.id;
        }
    };
    
    struct Frame
    {
        ID id{InvalidID};
        std::map<Slot, std::set<PlayerOutput>> outputs;
        
        Frame(ID _id)
        : id(_id)
        {
            
        }
        
        static Frame serialize(ReadStream& _stream)
        {
            //#TODO:
        }
        
        static WriteStream serialize(const Frame& _frame)
        {
            //#TODO:
        }
    };
}
