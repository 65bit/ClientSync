#pragma once

#include "ConfigReader.hpp"

class GameConfig
: public ConfigReader
{
    using Parent = ConfigReader;
    
public:
    struct Gun
    {
        float rotationSpeed{400.0f};
        int shootDelayMin{2};
        int shootDelayMax{5};
        int damage{20};
    };
    
    struct Player
    {
        int maxHealth{100};
        float moveSpeed{800.0f};
    }
    
public:
    Gun getGun() const
    {
        return {};
    }
    
    Player getPlayer() const
    {
        return {};
    }
    
private:
    bool process(rapidjson::Document& _doc) override
    {
        return true;
    }
}
