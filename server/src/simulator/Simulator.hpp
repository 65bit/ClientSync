#pragma once

#include <memory>
#include <vector>

#include "simulator/Types.hpp"
#include "simulator/Player.hpp"

namespace simulator
{
    class Simulator
    {
        using PlayerPtr = std::shared_ptr<Player>;
        
    public:
        Simulator(const Config& _config, const LoggerProxy& _logger)
        : m_config(_config)
        , m_logger(_logger)
        {
            
        }
        
        void simulate(unsigned _delta)
        {
            for(auto [slot, player] : m_players)
            {
                simulatePlayer(player, _delta);
            }
        }
        
        Frame getLastFrame() const
        {
            if(!m_frames.empty())
            {
                auto it = m_frames.begin();
                std::advance(it, m_frames.size() - 1);
                
                return *it;
            }
            
            return {InvalidID};
        }
        
        Frame buildFrame()
        {
            static ID nextFrameID = InvalidID;
            Frame frame{++nextFrameID};

            for(auto [slot, player] : m_players)
            {
                frame.outputs[slot] = player->popOutput();
            }

            m_frames.insert(frame);
            return frame;
        }
        
        PlayerPtr addPlayer()
        {
            static ID nextPlayerID = InvalidID;

            std::vector<Slot> order
            {
                Slot::Bottom,
                Slot::Top,
                Slot::Left,
                Slot::Right
            };
            
            PlayerPtr player;
            
            for(auto slot : order)
            {
                if(!m_players.count(slot))
                {
                    player = std::make_shared<Player>(++nextPlayerID, slot);
                    player->setPosition(getInitialPosition(slot));
                    
                    break;
                }
            }
            
            if(player)
            {
                m_players[player->getSlot()] = player;
            }
            
            return player;
        }
        
    private:
        void simulatePlayer(PlayerPtr _player, unsigned _delta) const
        {
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
        }
        
        bool directionValidForSlot(Direction _direction, Slot _slot) const
        {
            switch(_slot)
            {
                case Slot::Top:
                    [[fallthrough]];
                case Slot::Bottom:
                    return _direction == Direction::Left || _direction == Direction::Right;
                    
                case Slot::Left:
                    [[fallthrough]];
                case Slot::Right:
                    return _direction == Direction::Up || _direction == Direction::Down;
                    
                default:
                    break;
            }
            
            return false;
        }
        
        Vec2 getInitialPosition(Slot _slot) const
        {
            Vec2 result;
            
            switch(_slot)
            {
                case Slot::Top:
                    result.x = m_config.field.width / 2.0f;
                    result.y = m_config.field.height - m_config.player.platformHeight;
                    break;
                    
                case Slot::Bottom:
                    result.x = m_config.field.width / 2.0f;
                    result.y = m_config.player.platformHeight;
                    break;
                    
                case Slot::Left:
                    result.x = m_config.player.platformHeight;
                    result.y = m_config.field.height / 2.0f;
                    break;
                    
                case Slot::Right:
                    result.x = m_config.field.width - m_config.player.platformHeight;
                    result.y = m_config.field.height / 2.0f;
                    break;
                    
                default:
                    break;
            }
            
            return result;
        }
        
        Vec2 getVectorFromDirection(Direction _direction) const
        {
            switch(_direction)
            {
                case Direction::Left: return Vec2{-1.0f, 0.0f};
                case Direction::Right: return Vec2{1.0f, 0.0f};
                case Direction::Up: return Vec2{0.0f, 1.0f};
                case Direction::Down: return Vec2{0.0f, -1.0f};
                    
                default:
                    break;
            }
            
            return Vec2{0.0f, 0.0f};
        }
        
    private:
        const Config m_config;
        const LoggerProxy m_logger;
        
        std::map<Slot, PlayerPtr> m_players;
        std::set<Frame> m_frames;
    };
}
