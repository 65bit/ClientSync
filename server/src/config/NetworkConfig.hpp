#pragma once

#include "JsonProcessor.hpp"

class NetworkConfig
: public JsonProcessor
{
    using Parent = JsonProcessor;
    
public:
    int getPort() const
    {
        return m_port;
    }
    
    int getMaxPeersCount() const
    {
        return m_maxPeers;
    }
    
private:
    bool processJson(rapidjson::Document& _doc) override
    {
        m_port = _doc["port"].GetInt();
        m_maxPeers = _doc["max_peers"].GetInt();
        
        return true;
    }
    
private:
    int m_port{0};
    int m_maxPeers{0};
};
