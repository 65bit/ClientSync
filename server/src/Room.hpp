#pragma once

#include "CommonTypes.hpp"
#include "simulator/Simulator.hpp"
#include "config/GameConfig.hpp"

class Room
{
public:
    Room(Network& _network)
        : m_network(_network)
    {
        
    }

    bool init()
    {
        if(!m_config.read("data/game_config.json"))
        {
            return false;
        }
        
        simulator::LoggerProxy proxy;
        proxy.info = [](const std::string& _msg) { LOG_INFO(_msg); };
        proxy.warning = [](const std::string& _msg) { LOG_WARNING(_msg); };
        proxy.error = [](const std::string& _msg) { LOG_ERROR(_msg); };
        
        m_simulator.reset(new simulator::Simulator(m_config.getConfig(), proxy));
        
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
        /*for(auto player : m_network.getAllPlayers())
        {
            player->simulate(_delta);
        }*/
        
        if(m_simulator)
        {
            m_simulator->simulate(_delta);
        }
    }

    void broadcast(unsigned _delta)
    {
        /*const auto players = m_network.getAllPlayers();
        
        WriteStream stream(256);
        stream << static_enum_cast(PacketType::Frame) << static_cast<std::int32_t>(players.size());
        
        for(auto player : players)
        {
            const auto frames = player->getProcessedFrames();
            stream << player->getID().value << static_cast<std::int32_t>(frames.size());
            
            for(auto frame : frames)
            {
                //#TODO: need to create another approach for decimal numbers packing
                stream << frame.id << frame.pos.x << frame.pos.y;
            }
            
            player->clearProcessedFrames();
        }
        
        m_network.broadcast(stream);*/
        
        const auto frame = m_simulator->buildFrame();
        m_network.broadcast(simulator::Frame::serialize(frame));
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
        /*std::int32_t packetType = static_enum_cast(PacketType::Undefined);
        _stream >> packetType;
        
        switch(static_cast<PacketType>(packetType))
        {
            case PacketType::InitRequest:
            {
                processInit(_stream, _player);
            }
                break;
                
            case PacketType::Frame:
            {
                processFrame(_stream, _player);
            }
                break;
                
            default:
                LOG_WARNING("Receive unknown message type:" + std::to_string(packetType));
                break;
        }*/
        
        //#TODO:
    }

    void processInit(ReadStream& _stream, Player& _player)
    {
        /*WriteStream stream(128);
        stream << static_enum_cast(PacketType::InitResponse) << _player.getID().value;
        
        const auto players = m_network.getAllPlayers();
        stream << static_cast<std::int32_t>(players.size());
        
        for(auto player : players)
        {
            const ServerFrame frame = player->getLastProcessedFrame();
            stream << player->getID().value << frame.id << frame.pos.x << frame.pos.y;
        }
        
        m_network.send(_player, stream);*/
        
        //#TODO:
    }
    
    void processFrame(ReadStream& _stream, Player& _player)
    {
        /*std::int32_t framesCount = 0;
        _stream >> framesCount;
        
        std::vector<ClientFrame> frames;
        frames.reserve(framesCount);
        
        for(;framesCount != 0; --framesCount)
        {
            ClientFrame frame;
            _stream >> frame.id >> frame.dir.x >> frame.dir.y;
            
            frames.push_back(frame);
        }
        
        _player.pushUnprocessedFrames(std::move(frames));*/
        
        //#TODO:
    }
    
private:
    Network& m_network;
    GameConfig m_config;
    std::unique_ptr<simulator::Simulator> m_simulator;
};
