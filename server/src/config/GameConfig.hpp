#pragma once

#include "ConfigReader.hpp"
#include "simulator/Types.hpp"

class GameConfig
: public ConfigReader
{
    using Parent = ConfigReader;

public:
    simulator::Config getConfig() const
    {
        return m_config;
    }

private:
    bool process(rapidjson::Document& _doc) override
    {
        return processGun(_doc) && processPlayer(_doc) && processField(_doc);
    }
    
    bool processGun(rapidjson::Document& _doc)
    {
        const auto& value = _doc["gun"];

        m_config.gun.rotationSpeed = value["rotation_speed"].GetDouble();
        m_config.gun.damage = value["damage"].GetUint();
        m_config.gun.shootDelayMax = value["shoot_delay_max"].GetUint();
        m_config.gun.shootDelayMin = value["shoot_delay_min"].GetUint();
        
        return true;
    }
    
    bool processPlayer(rapidjson::Document& _doc)
    {
        const auto& value = _doc["player"];
        
        m_config.player.maxHealth = value["max_health"].GetUint();
        m_config.player.moveSpeed = value["move_speed"].GetDouble();
        m_config.player.platformHeight = value["platform_width"].GetDouble();
        m_config.player.platformWidth = value["platform_height"].GetDouble();
        
        return true;
    }
    
    bool processField(rapidjson::Document& _doc)
    {
        const auto& value = _doc["field"];
        
        m_config.field.width = value["field_width"].GetDouble();
        m_config.field.height = value["field_height"].GetDouble();
        
        return true;
    }
    
private:
    simulator::Config m_config;
};
