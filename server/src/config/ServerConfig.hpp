#pragma once

#include "ConfigReader.hpp"

class ServerConfig
: public ConfigReader
{
    using Parent = ConfigReader;
    
public:
    unsigned getSimulationRate() const
    {
        return m_simulationRate;
    }
    
    unsigned getBroadcastRate() const
    {
        return m_broadcastRate;
    }
    
private:
    bool process(rapidjson::Document& _doc) override
    {
        m_simulationRate = _doc["simulation_rate"].GetUint();
        m_broadcastRate = _doc["broadcast_rate"].GetUint();
        
        return true;
    }
    
private:
    unsigned m_simulationRate{0u};
    unsigned m_broadcastRate{0u};
};
