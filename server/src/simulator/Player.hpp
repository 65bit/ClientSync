#pragma once

namespace simulator
{
    class Player
    { 
    public:
        Player(ID _id, Slot _slot)
        : m_id(_id)
        , m_slot(_slot)
        {
            
        }
        
        bool hasUnprocessedInput() const
        {
            return !m_input.empty();
        }
        
        PlayerInput popPlayerInput()
        {
            if(!hasUnprocessedInput())
            {
                return {InvalidID};
            }
            
            const PlayerInput input = *m_input.begin();
            m_input.erase(m_input.begin());
            
            return input;
        }
        
        void pushInput(std::set<PlayerInput>&& _frames)
        {
            m_input.insert(_frames.begin(), _frames.end());
        }
        
        void pushOutput(const PlayerOutput& _output)
        {
            m_output.insert(_output);
        }
        
        std::set<PlayerOutput>&& popOutput()
        {
            return std::move(m_output);
        }
        
        void setPosition(const Vec2& _pos)
        {
            m_pos = _pos;
        }
        
        Vec2 getPosition() const
        {
            return m_pos;
        }
        
        ID getID() const
        {
            return m_id;
        }
        
        Slot getSlot() const
        {
            return m_slot;
        }
        
    private:
        /*Input popNextInput()
        {
            const Input input = *m_input.begin();
            m_input.erase(m_input.begin());
            
            return input;
        }*/
        
    private:
        const ID m_id{InvalidID};
        const Slot m_slot{Slot::Undefined};
        
        std::set<PlayerInput> m_input;
        std::set<PlayerOutput> m_output;
        
        Vec2 m_pos;
    };
}
