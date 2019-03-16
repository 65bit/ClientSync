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
        for(auto player : m_network.getAllPlayers())
        {
            player->simulate(_delta);
        }
    }

    void broadcast(unsigned _delta)
    {
        for(auto player : m_network.getAllPlayers())
        {
            WriteStream stream(128);
            stream << static_cast<std::int32_t>(m_network.getAllPlayers().size()) - 1;
            
            for(auto otherPlayer : m_network.getAllPlayers())
            {
                if(otherPlayer == player)
                {
                    continue;
                }
                
                stream << otherPlayer->getID().value;
            }
            
            const auto frames = player->getProcessedFrames();
            stream << (frames.empty() ? 0 : 1);
            
            if(!frames.empty())
            {
                const ServerFrame& lastProcessed = frames.back();
                stream << lastProcessed.id << lastProcessed.pos.x << lastProcessed.pos.y;
            }
            
            auto playersToSend = m_network.getAllPlayers();
            auto it = std::remove_if(playersToSend.begin(), playersToSend.end(), [player](Player* _player)
                                     {
                                         return _player == player || _player->getProcessedFrames().empty();
                                     });
            
            if(it != playersToSend.end())
            {
                playersToSend.erase(it, playersToSend.end());
            }
            
            stream << static_cast<std::int32_t>(playersToSend.size());
            
            if(!playersToSend.empty())
            {
                for(auto remotePlayer : playersToSend)
                {
                    const auto remoteFrames = remotePlayer->getProcessedFrames();
                    stream << remotePlayer->getID().value << static_cast<std::int32_t>(remoteFrames.size());
                    
                    for(const auto& remoteFrame : remoteFrames)
                    {
                        stream << remoteFrame.id << remoteFrame.pos.x << remoteFrame.pos.y;
                    }
                }
            }
            
            m_network.send(*player, stream);
        }
        
        for(auto player : m_network.getAllPlayers())
        {
            player->clearProcessedFrames();
        }
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
        std::int32_t framesCount = 0;
        _stream >> framesCount;
        
        std::vector<ClientFrame> frames;
        frames.reserve(framesCount);
        
        while(framesCount != 0)
        {
            ClientFrame frame;
            _stream >> frame.id >> frame.dir.x >> frame.dir.y;
            frames.push_back(frame);
            
            --framesCount;
        }
        
        _player.pushUnprocessedFrames(std::move(frames));
    }

private:
    Network& m_network;
};
