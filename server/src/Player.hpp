#pragma once

#include <vector>
#include <algorithm>

struct Vec2
{
    float x{ 0.0f };
    float y{ 0.0f };
    
    Vec2() = default;
    
    Vec2(float _x, float _y)
    : x(_x)
    , y(_y)
    {
        
    }
    
    Vec2 operator + (const Vec2& _vec)
    {
        return {x + _vec.x, y + _vec.y};
    }
};

struct ClientFrame
{
    std::int32_t id{0};
    Vec2 dir;
};

struct ServerFrame
{
    std::int32_t id{0};
    Vec2 pos;
};

class Player
{
    using ClientFrames = std::vector<ClientFrame>;
    using ServerFrames = std::vector<ServerFrame>;
    
public:
    struct ID
    {
        std::int32_t value{ 0 };

        ID(int _value)
            : value(_value)
        {

        }

        bool operator == (const ID& _id) const
        {
            return value == _id.value;
        }
    };

public:
    Player(const ID& _id)
        : m_id(_id)
    {
        m_unprocessedFrames.reserve(256);
    }

    ID getID() const
    {
        return m_id;
    }
    
    void pushUnprocessedFrames(std::vector<ClientFrame>&& _frames)
    {
        m_unprocessedFrames.insert(m_unprocessedFrames.end(), _frames.begin(), _frames.end());
        std::sort(m_unprocessedFrames.begin(), m_unprocessedFrames.end(), [](const ClientFrame& _l, const ClientFrame& _r)
                  {
                      return _l.id < _r.id;
                  });
    }
    
    ServerFrames getProcessedFrames() const
    {
        return m_processedFrames;
    }
    
    void simulate(unsigned _delta)
    {
        if(m_unprocessedFrames.empty())
        {
            return;
        }
        
        ClientFrame frame = *m_unprocessedFrames.begin();
        m_unprocessedFrames.erase(m_unprocessedFrames.begin());
        
        m_pos = m_pos + frame.dir;
        
        ServerFrame serverFrame;
        serverFrame.id = frame.id;
        serverFrame.pos = m_pos;
        
        m_lastProcessedFrame = serverFrame;
        m_processedFrames.push_back(serverFrame);
    }
    
    ServerFrame getLastProcessedFrame() const
    {
        return m_lastProcessedFrame;
    }
    
    void clearProcessedFrames()
    {
        m_processedFrames.clear();
    }
    
private:
    const ID m_id;
    
    ClientFrames m_unprocessedFrames;
    ServerFrames m_processedFrames;
    ServerFrame m_lastProcessedFrame;
    
    Vec2 m_pos;
};
