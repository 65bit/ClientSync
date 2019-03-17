#pragma once

#include "Network.hpp"
#include "Room.hpp"
#include "config/ServerConfig.hpp"

class Server
{
public:
    Server()
        : m_room(m_network)
    {

    }

    bool init()
    {
        if (!m_network.init())
        {
            return false;
        }

        if (!m_room.init())
        {
            return false;
        }
        
        if(!m_config.read("data/server_config.json"))
        {
            LOG_WARNING("Unnable to reade server config");
            return false;
        }

        LOG_INFO("Server initialized successfully");
        return true;
    }

    void deinit()
    {
        m_room.deinit();
        m_network.deinit();

        LOG_INFO("Server deinitialized successfully");
    }

    void run()
    {
        LOG_ENDL();
        LOG_INFO("Run server loop");
        LOG_INFO("Simulation rate:" + std::to_string(m_config.getSimulationRate()) + "ms");
        LOG_INFO("Broadcast rate:" + std::to_string(m_config.getBroadcastRate()) + "ms");

        auto simulateTime = m_network.getTime();
        auto broadcastTime = m_network.getTime();

        while (true)
        {
            auto delta = m_network.getTime() - simulateTime;

            if (delta >= m_config.getSimulationRate())
            {
                if (!m_network.poll(5))
                {
                    break;
                }

                m_room.simulate(delta);
                simulateTime = m_network.getTime();
            }

            delta = m_network.getTime() - broadcastTime;

            if (delta >= m_config.getBroadcastRate())
            {
                m_room.broadcast(delta);
                broadcastTime = m_network.getTime();
            }
        }
    } 

private:
    ServerConfig m_config;
    Network m_network;
    Room m_room;
};
