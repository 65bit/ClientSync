#pragma once

#include <enet/enet.h>

#include "Logger.hpp"
#include "Player.hpp"
#include "Callback.hpp"
#include "ReadStream.hpp"
#include "WriteStream.hpp"
#include "config/NetworkConfig.hpp"

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
        
        if(!m_config.read("data/network_config.json"))
        {
            LOG_ERROR("Unnable to read network config");
            return false;
        }

        ENetAddress address;
        address.host = ENET_HOST_ANY;
        address.port = m_config.getPort();

        m_host = enet_host_create(&address, m_config.getMaxPeersCount(), 2, 0, 0);

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
        enet_deinitialize();

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

    void send(Player& _receiver, const WriteStream& _stream, bool _reliable = true)
    {
        auto peer = getPeerForPlayer(_receiver);
        auto packet = enet_packet_create(_stream.getBuffer().data(), _stream.getBuffer().size(), preparePacketFlags(_reliable));

        enet_peer_send(peer, 0, packet);
    }

    void broadcast(const WriteStream& _stream, bool _reliable = true)
    {
        auto packet = enet_packet_create(_stream.getBuffer().data(), _stream.getBuffer().size(), preparePacketFlags(_reliable));
        enet_host_broadcast(m_host, 0, packet);
    }

    Player* getPlayer(Player::ID _id)
    {
        auto it = std::find_if(m_host->peers, m_host->peers + m_host->peerCount, [_id, this](const ENetPeer& _peer)
        {
            auto player = getPlayerForPeer(_peer);
            
            if(!player)
            {
                return false;
            }
            
            return player->getID() == _id;
        });

        return getPlayerForPeer(*it);
    }

    std::vector<Player*> getAllPlayers()
    {
        std::vector<Player*> result;
        result.reserve(m_host->peerCount);

        for (size_t i = 0; i < m_host->peerCount; ++i)
        {
            auto peer = m_host->peers[i];
            auto player = getPlayerForPeer(peer);
            
            if(player)
            {
                result.push_back(player);
            }
        }

        return result;
    }

    unsigned getTime() const
    {
        return enet_time_get();
    }

private:
    void initPlayer(ENetPeer* _peer)
    {
        static int nextFreeID = 0;
        auto player = createPlayerForPeer(_peer, ++nextFreeID);		

        LOG_INFO("Player initialized, ID:" + std::to_string(player->getID().value));
        onPlayerConnected(*player);
    }

    void deinitPlayer(ENetPeer* _peer)
    {
        auto player = getPlayerForPeer(_peer);

        if(!player)
        {
            return;
        }
        
        LOG_INFO("Player deinitialized, ID:" + std::to_string(player->getID().value));
        onPlayerDisconnected(*player);

        destroyPlayerForPeer(_peer);
    }

    void processPacket(ENetPeer* _sender, ENetPacket* _packet)
    {
        auto player = getPlayerForPeer(_sender);
        ReadStream stream{ {_packet->data, _packet->data + _packet->dataLength} };

        LOG_INFO("Processing packet from player, ID:" + std::to_string(player->getID().value));
        onPacketReceived(*player, stream);

        enet_packet_destroy(_packet);
    }

    // --------- Peer-Player management functionality ---------

    Player* createPlayerForPeer(ENetPeer* _peer, Player::ID _id)
    {
        auto player = new Player{ _id };
        _peer->data = player;
        return player;
    }

    void destroyPlayerForPeer(ENetPeer* _peer)
    {
        auto player = getPlayerForPeer(_peer);
        _peer->data = nullptr;
        delete player;
    }

    ENetPeer* getPeerForPlayer(Player& _player)
    {
        auto it = std::find_if(m_host->peers, m_host->peers + m_host->peerCount, [&_player, this](ENetPeer& _peer)
        {
            auto player = getPlayerForPeer(_peer);
            
            if(!player)
            {
                return false;
            }
            
            return player->getID() == _player.getID();
        });

        return &(*it);
    }

    Player* getPlayerForPeer(const ENetPeer* _peer)
    {
        return static_cast<Player*>(_peer->data);
    }

    Player* getPlayerForPeer(const ENetPeer& _peer)
    {
        return getPlayerForPeer(&_peer);
    }

    // --------- Other functionality ---------

    ENetPacketFlag preparePacketFlags(bool _reliable)
    {
        return _reliable ? ENET_PACKET_FLAG_RELIABLE : ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT;
    }

public:
    Callback<void(Player&)> onPlayerConnected;
    Callback<void(Player&)> onPlayerDisconnected;
    Callback<void(Player&, ReadStream)> onPacketReceived;

private:
    ENetHost* m_host;
    NetworkConfig m_config;
    
};
