#pragma once

#include <fstream>

#include "JsonProcessor.hpp"
#include "Level.hpp"
#include "CommonTypes.hpp"
#include "config/RoomConfig.hpp"

class Room
{
	class LevelReader
		: public JsonProcessor
	{
		using Parent = JsonProcessor;

	public:
		Level::StaticGeometry getStaticGeometry() const
		{
			return m_staticGeometry;
		}

	private:
		bool processJson(rapidjson::Document& _doc) override
		{
			//#TODO:
		}

	private:
		Level::StaticGeometry m_staticGeometry;
	};
public:
    Room(Network& _network)
        : m_network(_network)
    {
        
    }

    bool init()
    {
        if(!m_config.read("data/room_config.json"))
        {
            return false;
        }
     
		if (!initLevel())
		{
			return;
		}

		m_lastSimulateTimeStamp = m_network.getTime();
		m_lastBroadcastTimeStamp = m_network.getTime();

		LOG_INFO("Room initialized successfully");
		return true;
    }

    void deinit()
    {
        /*m_network.onPlayerConnected.remove(this, &Room::onPlayerConnected);
        m_network.onPlayerDisconnected.remove(this, &Room::onPlayerDisconnected);
        m_network.onPacketReceived.remove(this, &Room::onPacketReceived);

        LOG_INFO("Room deinitialized");*/
    }

    void simulate(unsigned _delta)
    {
		/*const float delta = 1.0f / static_cast<float>(_delta);

        for(auto player : m_network.getAllPlayers())
        {
			simulatePlayer(player, delta);
        }*/

		m_lastSimulateTimeStamp = m_network.getTime();
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

		m_lastBroadcastTimeStamp = m_network.getTime();
    }

	bool joinRoom(Player& _player)
	{
		/*std::vector<common::Slot> order
		{
			common::Slot::Bottom,
			common::Slot::Top,
			common::Slot::Left,
			common::Slot::Right
		};

		for (auto slot : order)
		{
			if (!m_players.count(slot))
			{
				//#TODO: Setup initial position
				m_players[slot] = _player.getID();

				return true;
			}
		}

		return false;*/

		return false;
	}

	unsigned getLastSimulateTimeStamp() const
	{
		return m_lastSimulateTimeStamp;
	}

	unsigned getLastBroadcastTimeStamp() const
	{
		return m_lastBroadcastTimeStamp;
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
        /*std::int32_t packetType = static_enum_cast(common::PacketType::Undefined);
        _stream >> packetType;
        
        switch(static_cast<common::PacketType>(packetType))
        {
            case common::PacketType::InitRequest:
            {
                processInit(_stream, _player);
            }
                break;
                
            case common::PacketType::Frame:
            {
                processFrame(_stream, _player);
            }
                break;
                
            default:
                LOG_WARNING("Receive unknown message type:" + std::to_string(packetType));
                break;
        }*/
    }

    void processInit(ReadStream& _stream, Player& _player)
    {
		/*if (registerPlayer(_player))
		{
			constexpr std::int32_t SnapshotIncluded = 1;
			constexpr std::int32_t SnapshotNotIncluded = 0;

			WriteStream stream(256);
			stream << _player.getID();

			if (!m_snapshots.empty())
			{
				auto it = m_snapshots.begin();
				std::advance(it, m_snapshots.size() - 1);

				WriteStream snapshot{ serialize(*it) };

				stream << SnapshotIncluded;
				stream.append(snapshot);
			}
			else
			{
				stream << SnapshotNotIncluded;
			}

			m_network.send(_player, stream);
		}
		else
		{
			//#TODO: Disconnect player, or process somehow else
		}*/
    }
    
    void processFrame(ReadStream& _stream, Player& _player)
    {
		/*std::set<common::ClientFrame> frames;

        std::int32_t framesCount = 0;
        _stream >> framesCount;
        
        for(;framesCount != 0; --framesCount)
        {
			common::ID id{ common::InvalidID };
			_stream >> id;

			common::ClientFrame frame{ id };
            _stream >> frame.dir.x >> frame.dir.y;
            
            frames.insert(frame);
        }
        
        _player.pushClientFrames(std::move(frames));*/
    }
    
	void simulatePlayer(Player* _player, float _delta)
	{
		/*
		if(!_player->hasUnprocessedInput())
        {
            return;
        }
            
        const PlayerInput input = _player->popPlayerInput();
            
        PlayerOutput output{input.id};
        output.pos = _player->getPosition();
            
        if(directionValidForSlot(input.dir, _player->getSlot()))
        {
            const Vec2 vec = getVectorFromDirection(input.dir);
            const Vec2 velocity = vec * m_config.player.moveSpeed * (1.0f / static_cast<float>(_delta));
            const Vec2 pos = _player->getPosition() + velocity;
                
            output.pos = pos;
        }
            
        _player->pushOutput(output);
		*/
	}

	WriteStream&& serialize(const common::Snapshot& _snapshot) const
	{
		/*WriteStream stream(256);
		
		//Serialize frames
		stream << static_cast<std::int32_t>(_snapshot.frames.size());

		for (const auto& pair : _snapshot.frames)
		{
			const common::ID id = pair.first;
			const auto& frames = pair.second;

			stream << id << static_cast<std::int32_t>(frames.size());

			for (const common::ServerFrame& frame : frames)
			{
				stream << frame.id << frame.pos.x << frame.pos.y;
			}
		}

		return std::move(stream);*/

		return{0};
	}

	bool initLevel()
	{
		LevelReader reader;

		if(reader.read("#TODO"))
		{
			m_level.init(reader.getStaticGeometry());
			return true;
		}

		return false;
	}

private:
	unsigned m_lastSimulateTimeStamp{ 0 };
	unsigned m_lastBroadcastTimeStamp{ 0 };

	Network& m_network;
    RoomConfig m_config;
	Level m_level;

	//std::map<common::Slot, common::ID> m_players;
	//std::set<common::Snapshot> m_snapshots;
};
