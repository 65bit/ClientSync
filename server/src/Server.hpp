#pragma once

#include <memory>

#include "Network.hpp"
#include "Room.hpp"
#include "config/ServerConfig.hpp"

class Server
{
	using RoomPtr = std::unique_ptr<Room>;
	using Rooms = std::vector<RoomPtr>;

public:
    Server()
    {

    }

    bool init()
    {
        if (!m_network.init())
        {
            return false;
        }

        if(!m_config.read("data/server_config.json"))
        {
            return false;
        }

		if (!initRooms())
		{
			return false;
		}

		m_network.onPlayerConnected.add(this, &Server::onPlayerConnected);
		m_network.onPlayerDisconnected.add(this, &Server::onPlayerDisconnected);
		m_network.onPacketReceived.add(this, &Server::onPacketReceived);

        LOG_INFO("Server initialized successfully");
        return true;
    }

    void deinit()
    {
		m_network.onPlayerConnected.remove(this, &Server::onPlayerConnected);
		m_network.onPlayerDisconnected.remove(this, &Server::onPlayerDisconnected);
		m_network.onPacketReceived.remove(this, &Server::onPacketReceived);

		for (auto& room : m_rooms)
		{
			room->deinit();
		}

        m_network.deinit();
        LOG_INFO("Server deinitialized successfully");
    }

    void run()
    {
        LOG_ENDL();
        LOG_INFO("Run server loop");
        LOG_INFO("Simulation rate:" + std::to_string(m_config.getSimulationRate()) + "ms");
        LOG_INFO("Broadcast rate:" + std::to_string(m_config.getBroadcastRate()) + "ms");
		LOG_INFO("Rooms cache:" + std::to_string(m_config.getRoomsCacheSize()) + "ms");

        while (true)
        {
			if (m_network.poll(5))
			{
				for (auto& room : m_rooms)
				{
					auto delta = m_network.getTime() - room->getLastSimulateTimeStamp();

					if (delta >= m_config.getSimulationRate())
					{
						room->simulate(delta);
					}

					delta = m_network.getTime() - room->getLastBroadcastTimeStamp();

					if (delta >= m_config.getBroadcastRate())
					{
						room->broadcast(delta);
					}
				}
			}
			else
			{
				//#TODO: Process error and shutdown server
			}
        }
    }

private:
	void onPlayerConnected(Player& _player)
	{
		if (!joinRoom(_player))
		{
			//#TODO: Server is full. Kick player
			LOG_WARNING("#TODO:Kick just connected player because server is full");
		}		
	}

	void onPlayerDisconnected(Player& _player)
	{
		_player.onDisconnected();
	}

	void onPacketReceived(Player& _player, ReadStream _stream)
	{
		_player.onPacketReceived(_stream);
	}

	bool initRooms()
	{
		const auto cacheSize = m_config.getRoomsCacheSize();
		m_rooms.reserve(cacheSize);

		for (std::int32_t i = 0; i < cacheSize; ++i)
		{
			RoomPtr room{ new Room{ m_network } };

			if (room->init())
			{
				m_rooms.push_back(std::move(room));
			}
			else
			{
				LOG_WARNING("Unnable to initialize cached room");
				return false;
			}
		}

		return true;
	}

	bool joinRoom(Player& _player)
	{
		//#TODO: Join most populated room

		for (auto& room : m_rooms)
		{
			if (room->joinRoom(_player))
			{
				return true;
			}
		}

		return false;
	}

private:
    ServerConfig m_config;
    Network m_network;
	Rooms m_rooms;
};
