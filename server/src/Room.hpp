#pragma once

class Room
{
public:
    Room(Network& _network)
        : m_network(_network)
    {
        
    }

    bool init()
    {
        m_network.onPlayerConnected.add(this, &Room::onPlayerConnected);
        m_network.onPlayerDisconnected.add(this, &Room::onPlayerDisconnected);
        m_network.onPacketReceived.add(this, &Room::onPacketReceived);

        LOG_INFO("Room initialized successfully");
        return true;
    }

    void deinit()
    {
        m_network.onPlayerConnected.remove(this, &Room::onPlayerConnected);
        m_network.onPlayerDisconnected.remove(this, &Room::onPlayerDisconnected);
        m_network.onPacketReceived.remove(this, &Room::onPacketReceived);

        LOG_INFO("Room deinitialized");
    }

    void simulate(unsigned _delta)
    {

    }

    void broadcast(unsigned _delta)
    {

    }

protected:
    void onPlayerConnected(Player& _player) 
    {

    }

    void onPlayerDisconnected(Player& _player)
    {

    }
    
    void onPacketReceived(Player& _player, ReadStream _stream)
    {

    }

private:
    Network& m_network;
};