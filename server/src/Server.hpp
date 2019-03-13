#pragma once

#include "Network.hpp"
#include "Room.hpp"

class Server
{
    static constexpr unsigned SimulationRate = 16u; // 60hz
    static constexpr unsigned BroadcastRate = 100u; // 10hz

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
        LOG_INFO("Simulation rate:" + std::to_string(SimulationRate) + "ms");
        LOG_INFO("Broadcast rate:" + std::to_string(BroadcastRate) + "ms");

        auto simulateTime = m_network.getTime();
        auto broadcastTime = m_network.getTime();

        while (true)
        {
            auto delta = m_network.getTime() - simulateTime;

            if (delta >= SimulationRate)
            {
                if (!m_network.poll(5))
                {
                    break;
                }

                m_room.simulate(delta);
                simulateTime = m_network.getTime();
            }

            delta = m_network.getTime() - broadcastTime;

            if (delta >= BroadcastRate)
            {
                m_room.broadcast(delta);
                broadcastTime = m_network.getTime();
            }
        }
    } 

private:
    Network m_network;
    Room m_room;
};