#include "Connection.hpp"

using namespace cocos2d;

namespace
{
    void DestroyENetHost(ENetHost* _host)
    {
        if(!_host)
        {
            enet_host_destroy(_host);
        }
    }
    
    void DestroyENetPeer(ENetPeer* _peer)
    {
        //Nothing, ENet will take handle of peer deallocation
    }
}

// Command .........

Connection::Connection()
: m_host(nullptr, std::bind(DestroyENetHost, std::placeholders::_1))
, m_peer(nullptr, std::bind(DestroyENetPeer, std::placeholders::_1))
{
    
}

Connection::Connection(Connection&& _movable)
{
    m_host = std::move(_movable.m_host);
    m_peer = std::move(_movable.m_peer);
    m_state = _movable.m_state;
    
    onStateChanged = _movable.onStateChanged;
    onPacketReceived = _movable.onPacketReceived;
    
    _movable.onStateChanged.clear();
    _movable.onPacketReceived.clear();
    _movable.m_state = State::Undefined;
}

Connection::~Connection()
{
    terminate(false);
}

void Connection::poll()
{
    if(m_state == State::Undefined)
    {
        return;
    }
    
    int iterationsCount = 5;
    
    std::vector<ENetEvent> collectedEvents;
    collectedEvents.reserve(iterationsCount);
    
    ENetEvent event;
    
    while(iterationsCount != 0)
    {
        int serviceResult = enet_host_service(m_host.get(), &event, 0);
        
        if(serviceResult == 0)
        {
            //No events
            break;
        }
        
        if(serviceResult < 0)
        {
            CCLOG("Some unknown error occured, terminating connection");
            terminate(true);
            
            return;
        }
        
        collectedEvents.push_back(event);
        --iterationsCount;
    }
    
    for(const auto& event : collectedEvents)
    {
        switch(event.type)
        {
            case ENET_EVENT_TYPE_CONNECT:
            {
                CCLOG("Receive ENET_EVENT_TYPE_CONNECT event");
                setState(State::Connected);
            }
                break;
                
            case ENET_EVENT_TYPE_DISCONNECT:
            {
                CCLOG("Receive ENET_EVENT_TYPE_DISCONNECT event");
                setState(State::Disconnected);
            }
                break;
                
            case ENET_EVENT_TYPE_RECEIVE:
            {
                CCLOG("Receive ENET_EVENT_TYPE_RECEIVE event");
                ReadStream stream{{event.packet->data, event.packet->data + event.packet->dataLength}};
                
                onPacketReceived(stream);
                enet_packet_destroy(event.packet);
            }
                break;
                
            default:
                CCLOG("Receive unknown ENet event");
                break;
        }
    }
}

bool Connection::connect()
{
    switch(m_state)
    {
        case State::Undefined:
        {
            if(enet_initialize())
            {
                CCLOG("Unnable to initialize ENet library");
                return false;
            }
            
            setState(State::Initialized);
        }
            [[fallthrough]];
            
        case State::Initialized:
        {
            m_host.reset(enet_host_create(nullptr, 1, 2, 0, 0));
            
            if(!m_host)
            {
                CCLOG("Unnable to create ENet host");
                return false;
            }
            
            setState(State::Disconnected);
        }
            [[fallthrough]];
            
        case State::Disconnected:
        {
            ENetAddress address;
            enet_address_set_host_ip(&address, "127.0.0.1");
            address.port = 55555;
            
            m_peer.reset(enet_host_connect(m_host.get(), &address, 2, 0));
            
            if(!m_peer)
            {
                CCLOG("Unnable to initialize connection");
                return false;
            }
            
            setState(State::Connecting);
            return true;
        }
            break;
            
        default:
            break;
    }
    
    return false;
}

bool Connection::disconnect()
{
    if(m_state != State::Connected)
    {
        return false;
    }
    
    enet_peer_disconnect(m_peer.get(), 0);
    setState(State::Disconnecting);
    
    return true;
}

bool Connection::send(const WriteStream& _stream)
{
    if(m_state != State::Connected)
    {
        CCLOG("Trying to send data while state in %s state", ToString(m_state).c_str());
        return false;
    }
    
    auto packet = enet_packet_create(_stream.getBuffer().data(), _stream.getBuffer().size(), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(m_peer.get(), 0, packet);
    
    return true;
}

Connection::State Connection::getState() const
{
    return m_state;
}

void Connection::setState(State _newState, bool _invokeCallbacks)
{
    if(m_state == _newState)
    {
        return;
    }
    
    CCLOG("Changing connection state from:%s -> to:%s", ToString(m_state).c_str(), ToString(_newState).c_str());
    m_state = _newState;
    
    if(_invokeCallbacks)
    {
        onStateChanged(m_state);
    }
}

void Connection::terminate(bool _invokeCallbacks)
{
    if(m_peer)
    {
        enet_peer_disconnect_now(m_peer.get(), 0);
    }
    
    m_peer.reset();
    m_host.reset();
    
    if(m_state != State::Undefined)
    {
        enet_deinitialize();
    }
    
    setState(State::Undefined, _invokeCallbacks);
}

std::string Connection::ToString(State _state)
{
    switch(_state)
    {
        case State::Undefined: return "Undefined";
        case State::Initialized: return "Initialized";
        case State::Disconnected: return "Disconnected";
        case State::Connecting: return "Connecting";
        case State::Connected: return "Connected";
        case State::Disconnecting: return "Disconnecting";
            
        default:
            break;
    }
    
    return "-";
}
