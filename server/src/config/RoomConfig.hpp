#pragma once

#include "JsonProcessor.hpp"

class RoomConfig
: public JsonProcessor
{
    using Parent = JsonProcessor;

public:
	unsigned getMaxPlayersCount() const
	{
		return m_maxPlayersCount;
	}

private:
    bool processJson(rapidjson::Document& _doc) override
    {
		m_maxPlayersCount = _doc["max_players_count"].GetUint();
    }
    
private:
	unsigned m_maxPlayersCount{ 0 };
};
