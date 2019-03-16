#pragma once

#include <enet/enet.h>

#include "Core.hpp"
#include "WriteStream.hpp"
#include "ReadStream.hpp"

namespace connection
{
    class Command;
}

class Connection final
{
public:
    enum class State
    {
        Undefined,
        Initialized,
        Connecting,
        Connected,
        Disconnecting,
        Disconnected
    };
    
    static std::string ToString(State _state);
    
private:
    Connection(const Connection&) = delete;
    Connection& operator = (const Connection&) = delete;
    
public:
    Connection();
    Connection(Connection&& _movable);
    ~Connection();
    
    void poll();
    
    bool connect();
    bool disconnect();
    bool send(const WriteStream& _stream);
    
    State getState() const;
    
private:
    void setState(State _newState, bool _invokeCallbacks = true);
    void terminate(bool _invokeCallbacks);
    void sendPendingPackets();
    void releasePendingPackets();
    
public:
    Callback<void(State)> onStateChanged;
    Callback<void(ReadStream)> onPacketReceived;
    
private:
    State m_state{State::Undefined};
    
    std::unique_ptr<ENetHost, std::function<void(ENetHost*)>> m_host;
    std::unique_ptr<ENetPeer, std::function<void(ENetPeer*)>> m_peer;
    
    std::vector<ENetPacket*> m_pendingPackets;
};
