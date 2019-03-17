#pragma once

#include "ConfigReader.hpp"

class NetworkConfig
: public ConfigReader
{
    using Parent = ConfigReader;
    
public:
    int getPort() const
    {
        return 55555;
    }
    
    int getMaxPeersCount() const
    {
        return 1200;
    }
    
private:
    bool process(rapidjson::Document& _doc) override
    {
        return true;
    }
};
