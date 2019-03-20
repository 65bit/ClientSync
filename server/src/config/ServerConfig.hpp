#pragma once

#include "JsonProcessor.hpp"

class ServerConfig
: public JsonProcessor
{
    using Parent = JsonProcessor;
    
public:
    unsigned getSimulationRate() const
    {
        return m_simulationRate;
    }
    
    unsigned getBroadcastRate() const
    {
        return m_broadcastRate;
    }
    
	unsigned getRoomsCacheSize() const
	{
		return m_roomsCacheSize;
	}

private:
    bool processJson(rapidjson::Document& _doc) override
    {
        m_simulationRate = _doc["simulation_rate"].GetUint();
        m_broadcastRate = _doc["broadcast_rate"].GetUint();
		m_roomsCacheSize = _doc["rooms_cache_size"].GetUint();

        return true;
    }
    
private:
    unsigned m_simulationRate{0u};
	unsigned m_broadcastRate{0u};
	unsigned m_roomsCacheSize{0u};
};
