#include <vector>
#include <string>
#include <thread>
#include <iostream>

#include <enet/enet.h>

class Logger
{
public:
    enum class Level
    {
        Info,
        Warning,
        Error
    };

private:
    Logger()
    {

    }

    Logger(const Logger&) = delete;
    Logger& operator = (const Logger&) = delete;

public:
    static Logger& Instance()
    {
        static Logger instance;
        return instance;
    }

public:
    void log(const std::string& _message, Level _level = Level::Info)
    {
        std::string prefix;

        switch (_level)
        {
        case Level::Info:
            prefix = "[INFO]";
            break;

        case Level::Warning:
            prefix = "[WARNING]";
            break;
        
        case Level::Error:
            prefix = "[ERROR]";
            break;
        
        default:
            prefix = "[UNDEFINED]";
            break;
        }

        std::cout << prefix << _message << std::endl;
    }
};

#define LOG_INFO(__MESSAGE__) do{ std::string msg(__MESSAGE__); Logger::Instance().log(msg + " file:" + __FILE__ + " line:" + std::to_string(__LINE__), Logger::Level::Info); }while(0)
#define LOG_WARNING(__MESSAGE__) do{ std::string msg(__MESSAGE__); Logger::Instance().log(msg + " file:" +  __FILE__ + " line:" +  std::to_string(__LINE__), Logger::Level::Warning); }while(0)
#define LOG_ERROR(__MESSAGE__) do{ std::string msg(__MESSAGE__); Logger::Instance().log(msg + " file:" +  __FILE__ + " line:" +  std::to_string(__LINE__), Logger::Level::Error); }while(0)

class Client
{
public:
    struct ID
    {
        int value = 0;
    };
};

class Server
{
    static constexpr enet_uint32 SimulationRate = 16u; // 60hz
    static constexpr enet_uint32 BroadcastRate = 100u; // 10hz

public:
    Server()
		: m_host(nullptr)
    {

    }

    bool init(std::vector<std::string>&& _args)
    {
        const bool result = !enet_initialize();

        if (!result)
        {
            LOG_ERROR("Unnable to initialize ENet library");
            return false;
        }

        ENetAddress address;
        address.host = ENET_HOST_ANY;
        address.port = 56745;

        m_host = enet_host_create(&address, 32, 2, 0, 0);

        if (!m_host)
        {
            LOG_ERROR("Unnable to create ENet host");
            return false;
        }

        enet_time_set(enet_time_get());
        return true;
    }

    void run()
    {
        auto simulateTime = enet_time_get();
        auto broadcastTime = enet_time_get();

        while (true)
        {
            if ((enet_time_get() - simulateTime) >= SimulationRate)
            {
                if (!processEvents())
                {
                    break;
                }

                simulate();
                simulateTime = enet_time_get();
            }

            if ((enet_time_get() - broadcastTime) >= BroadcastRate)
            {
                broadcast();
                broadcastTime = enet_time_get();
            }
        }
    }

private:
    bool processEvents()
    {
        ENetEvent event;
        int maxIterations = 5;

        while (maxIterations != 0)
        {
            int serviceResult = enet_host_service(m_host, &event, maxIterations);

            if (serviceResult < 0)
            {
                LOG_ERROR("ENet host service returned with error");
                return false;
            }

            if (serviceResult == 0)
            {
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

            --maxIterations;
        }

        return true;
    }

    void simulate()
    {

    }

    void broadcast()
    {

    }

    void initPlayer(ENetPeer* _peer)
    {

    }

    void deinitPlayer(ENetPeer* _peer)
    {

    }

    void processPacket(ENetPeer* _sender, ENetPacket* _packet)
    {
        enet_packet_destroy(_packet);
    }

private:
    ENetHost* m_host;
};

int main(int _argc, char** _argv)
{
    Server server;

    if (server.init({ (*_argv, *_argv + _argc) }))
    {
        server.run();
    }

    return 0;
}