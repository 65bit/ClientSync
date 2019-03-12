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

        enet_time_set(enet_time_get());
        return true;
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

    unsigned getTime() const
    {
        return enet_time_get();
    }

private:
    void initPlayer(ENetPeer* _peer)
    {

    }

    void deinitPlayer(ENetPeer* _peer)
    {

    }

    void processPacket(ENetPeer* _sender, ENetPacket* _packet)
    {
        enet_packet_destroy(_packet);
    }

public:
    Callback<void(Player::ID)> onPlayerConnected;
    Callback<void(Player::ID)> onPlayerDisconnected;
    Callback<void(Player::ID, ReadStream)> onPacketReceived;

private:
    ENetHost* m_host;
};