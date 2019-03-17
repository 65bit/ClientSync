#pragma once

#include "ConfigReader.hpp"

class ServerConfig
: public ConfigReader
{
    using Parent = ConfigReader;
    
public:
    unsigned getSimulationRate() const
    {
        return 16u;
    }
    
    unsigned getBroadcastRate() const
    {
        return 100u;
    }
    
private:
    bool process(rapidjson::Document& _doc) override
    {
        return true;
    }
};
