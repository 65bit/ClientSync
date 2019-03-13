#pragma once

#include <enet/enet.h>

#include "Logger.hpp"
#include "Player.hpp"
#include "Callback.hpp"
#include "ReadStream.h"
#include "WriteStream.hpp"

class Network
{
public:
    bool init()
    {
        if (enet_initialize() != 0)
        {
            LOG_ERROR("Unnable to initialize ENet library");
            return false;
        }

        ENetAddress address;
        address.host = ENET_HOST_ANY;
        address.port = 56745;

        m_host = enet_host_create(&address, 32, 2, 0, 0);

        if (!m_host)
        {
            LOG_ERROR("Unnable to create ENet host");
            return false;
        }

		LOG_INFO("Network initialized successfully");
        enet_time_set(enet_time_get());

        return true;
    }

	void deinit()
	{
		std::for_each(m_host->peers, m_host->peers + m_host->peerCount, [this](ENetPeer& _peer)
		{
			deinitPlayer(&_peer);
			enet_peer_disconnect_now(&_peer, 0);
		});

		enet_host_destroy(m_host);
		LOG_INFO("Network deinitialized successfully");
	}

    bool poll(int _maxIterations)
    {
        ENetEvent event;

        while (_maxIterations != 0)
        {
            int serviceResult = enet_host_service(m_host, &event, 0);

            if (serviceResult < 0)
            {
                LOG_ERROR("ENet host service returned with error");
                return false;
            }

            if (serviceResult == 0)
            {
                // No events to process
                break;
            }

            switch (event.type)
            {
            case ENET_EVENT_TYPE_CONNECT:
            {
                initPlayer(event.peer);
            }
            break;

            case ENET_EVENT_TYPE_DISCONNECT:
            {
                deinitPlayer(event.peer);
            }
            break;

            case ENET_EVENT_TYPE_RECEIVE:
            {
                processPacket(event.peer, event.packet);
            }
            break;

            default:
            {
                LOG_ERROR("Unknown ENet event type");
                return false;
            }
            break;
            }

            --_maxIterations;
        }

        return true;
    }

	Player& getPlayer(Player::ID _id)
	{
		auto it = std::find_if(m_host->peers, m_host->peers + m_host->peerCount, [_id](const ENetPeer& _peer)
		{
			auto player = static_cast<Player*>(_peer.data);
			return player->getID() == _id;
		});

		return *(static_cast<Player*>(it->data));
	}

    unsigned getTime() const
    {
        return enet_time_get();
    }

private:
    void initPlayer(ENetPeer* _peer)
    {
		static int nextFreeID = 0;

		auto player = new Player{ Player::ID{nextFreeID} };
		_peer->data = player;

		LOG_INFO("Player initialized, ID:" + std::to_string(player->getID().value));
		onPlayerConnected(*player);
    }

    void deinitPlayer(ENetPeer* _peer)
    {
		auto player = static_cast<Player*>(_peer->data);
		_peer->data = nullptr;

		LOG_INFO("Player deinitialized, ID:" + std::to_string(player->getID().value));
		onPlayerDisconnected(*player);

		delete player;
    }

    void processPacket(ENetPeer* _sender, ENetPacket* _packet)
    {
		auto player = static_cast<Player*>(_sender->data);
		ReadStream stream{ {_packet->data, _packet->data + _packet->dataLength} };

		LOG_INFO("Processing packet from player, ID:" + std::to_string(player->getID().value));
		onPacketReceived(*player, stream);

        enet_packet_destroy(_packet);
    }

public:
    Callback<void(Player&)> onPlayerConnected;
    Callback<void(Player&)> onPlayerDisconnected;
    Callback<void(Player&, ReadStream)> onPacketReceived;

private:
    ENetHost* m_host;
};