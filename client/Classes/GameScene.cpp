#include "GameScene.hpp"
#include "ConnectScene.hpp"
#include "CommonTypes.hpp"

using namespace cocos2d;

GameScene::GameScene(Connection&& _connection)
: m_connection(std::move(_connection))
{
    
}

GameScene::~GameScene()
{
    
}

bool GameScene::init()
{
    if(!Parent::init())
    {
        return false;
    }
    
    const auto designSize = VIEW->getDesignResolutionSize();
    
    m_disconnectButton = ui::Button::create("64_white.png");
    m_disconnectButton->setPosition({0.0f, designSize.height});
    m_disconnectButton->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
    m_disconnectButton->setTitleFontSize(15.0f);
    m_disconnectButton->setTitleColor(Color3B::RED);
    m_disconnectButton->setTitleText("Disconnect");
    m_disconnectButton->addTouchEventListener(CC_CALLBACK_2(GameScene::onButtonTouch, this));
    addChild(m_disconnectButton);
    
    createLocalPlayer();
    createGun();
    scheduleUpdate();
    subscribe();
    
    WriteStream stream(10);
    stream << static_enum_cast(PacketType::InitRequest);
    
    m_connection.send(stream);
    return true;
}

void GameScene::update(float _delta)
{
    Parent::update(_delta);
    m_connection.poll();
    
    // Network update
    
    constexpr float networkTick = 1.0f / 30.0f;
    m_networkTicks += _delta;
    
    if(m_networkTicks >= networkTick)
    {
        auto frames = m_localPlayer->getFrames();
        
        if(frames.empty())
        {
            return;
        }
        
        WriteStream stream(frames.size() * sizeof(LocalFrame)); // Appox. capacity
        stream << static_enum_cast(PacketType::Frame) << static_cast<std::int32_t>(frames.size());
        
        for(const auto& frame : frames)
        {
            stream << frame.id << frame.dir.x << frame.dir.y;
        }
        
        m_connection.send(stream);
        m_networkTicks = 0.0f;
    }
}

void GameScene::onButtonTouch(cocos2d::Ref* _sender, cocos2d::ui::Widget::TouchEventType _type)
{
    if(_type != ui::Widget::TouchEventType::ENDED)
    {
        return;
    }
    
    if(_sender == m_disconnectButton)
    {
        returnToConnectScene();
    }
    else
    {
        CCLOG("Unknown button");
    }
}

void GameScene::returnToConnectScene()
{
    unsubscribe();
    m_connection.disconnect();
    DIRECTOR->replaceScene(ConnectScene::create(std::move(m_connection)));
}

void GameScene::onConnectionStateChanged(Connection::State _newState)
{
    returnToConnectScene();
}

void GameScene::onPacketReceived(ReadStream _stream)
{
    auto packetType = static_enum_cast(PacketType::Undefined);
    _stream >> packetType;
    
    switch(static_cast<PacketType>(packetType))
    {
        case PacketType::InitResponse:
            processInit(_stream);
            break;
            
        case PacketType::Frame:
            processFrame(_stream);
            break;
            
        default:
            CCLOG("Receive packet with unknown type");
            break;
    }
}

void GameScene::processInit(ReadStream& _stream)
{
    CCASSERT(m_localID == 0, "Client already initialized");
    CCLOG("Initializing client");
    
    _stream >> m_localID;

    std::int32_t playersCount = 0;
    _stream >> playersCount;

    for(;playersCount != 0; --playersCount)
    {
        std::int32_t playerID = 0;
        _stream >> playerID;

        ServerFrame frame;
        _stream >> frame.id >> frame.pos.x >> frame.pos.y;
        
        if(playerID != m_localID)
        {
            auto opponent = addOpponent(playerID);
            opponent->initFromFrame(frame);
        }
    }
}

void GameScene::processFrame(ReadStream& _stream)
{
    if(m_localID == 0)
    {
        return;
    }
    
    std::int32_t playersCount = 0;
    _stream >> playersCount;
    
    std::vector<std::int32_t> opponentsID;
    opponentsID.reserve(playersCount);
    
    for(;playersCount != 0; --playersCount)
    {
        std::int32_t playerID = 0;
        _stream >> playerID;
        
        std::int32_t framesCount = 0;
        _stream >> framesCount;
        
        std::vector<ServerFrame> serverFrames;
        serverFrames.reserve(framesCount);
        
        for(;framesCount != 0; --framesCount)
        {
            ServerFrame frame;
            _stream >> frame.id >> frame.pos.x >> frame.pos.y;
            
            serverFrames.push_back(frame);
        }
        
        if(playerID == m_localID)
        {
            if(!serverFrames.empty())
            {
                m_localPlayer->reconcile(serverFrames.back());
            }
        }
        else
        {
            pushOpponentFrames(playerID, std::move(serverFrames));
            opponentsID.push_back(playerID);
        }
    }
    
    checkOpponents(opponentsID);
}

void GameScene::checkOpponents(const std::vector<std::int32_t>& _id)
{
    //Check disconnected
    auto it = std::remove_if(m_opponents.begin(), m_opponents.end(), [&_id](Opponent* _opponent)
                             {
                                 auto it = std::find(_id.begin(), _id.end(), _opponent->getID());
                                 
                                 if(it == _id.end())
                                 {
                                     _opponent->removeFromParent();
                                 }
                                 
                                 return it == _id.end();
                             });
    
    if(it != m_opponents.end())
    {
        m_opponents.erase(it, m_opponents.end());
    }
}

Opponent* GameScene::addOpponent(std::int32_t _id)
{
    Opponent* opponent = Opponent::create(_id);
    addChild(opponent);
    
    m_opponents.push_back(opponent);
    return opponent;
}

void GameScene::pushOpponentFrames(std::int32_t _id, std::vector<ServerFrame>&& _frames)
{
    auto it = std::find_if(m_opponents.begin(), m_opponents.end(), [_id](Opponent* _opponent)
                           {
                               return _opponent->getID() == _id;
                           });
    
    Opponent* opponent = nullptr;
    
    if(it == m_opponents.end())
    {
        // New player connected
        opponent = addOpponent(_id);
        
        if(!_frames.empty())
        {
            opponent->initFromFrame(_frames.front());
        }
    }
    else
    {
        opponent = *it;
    }

    opponent->pushFrames(std::move(_frames));
}

void GameScene::createGun()
{
    m_gun = Gun::create();
    m_gun->setPosition(VIEW->getDesignResolutionSize() * 0.5f);
    addChild(m_gun);
}

void GameScene::createLocalPlayer()
{
    m_localPlayer = LocalPlayer::create();
    addChild(m_localPlayer);
}

void GameScene::subscribe()
{
    m_connection.onStateChanged.add(this, &GameScene::onConnectionStateChanged);
    m_connection.onPacketReceived.add(this, &GameScene::onPacketReceived);
}

void GameScene::unsubscribe()
{
    m_connection.onStateChanged.remove(this, &GameScene::onConnectionStateChanged);
    m_connection.onPacketReceived.remove(this, &GameScene::onPacketReceived);
}

// LocalPlayer .........

LocalPlayer::LocalPlayer()
{
    m_unackFrames.reserve(128);
    m_framesToSend.reserve(128);
}

void LocalPlayer::reconcile(const ServerFrame& _frame)
{
    auto it = std::find_if(m_unackFrames.begin(), m_unackFrames.end(), [_frame](const LocalFrame& _localFrame)
                           {
                               return _frame.id == _localFrame.id;
                           });
    
    constexpr float eps = 5.0f;
    const Vec2 predictedPos = it->oldPos + it->dir;
    
    m_unackFrames.erase(m_unackFrames.begin(), it + 1);
    
    if(predictedPos.distance(_frame.pos) >= eps)
    {
        CCLOG("Out of sync detected, reconcile");
        Vec2 pos = _frame.pos;
        
        for(auto& frame : m_unackFrames)
        {
            frame.oldPos = pos;
            pos += frame.dir;
        }
        
        setPosition(pos);
    }
}

bool LocalPlayer::init()
{
    if(!Parent::init())
    {
        return false;
    }
    
    setTexture("64_white.png");
    setColor(Color3B::RED);
    
    m_keyboardListener = EventListenerKeyboard::create();
    m_keyboardListener->onKeyPressed = CC_CALLBACK_2(LocalPlayer::onKeyPressed, this);
    m_keyboardListener->onKeyReleased = CC_CALLBACK_2(LocalPlayer::onKeyReleased, this);
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(m_keyboardListener, this);

    scheduleUpdate();
    return true;
}

void LocalPlayer::update(float _delta)
{
    Parent::update(_delta);
    
    constexpr float speed = 200.0f;
    Vec2 movementVec;
    
    if(isKeyPressed(EventKeyboard::KeyCode::KEY_A) || isKeyPressed(EventKeyboard::KeyCode::KEY_KP_LEFT))
    {
        movementVec += {-1.0f, 0.0f};
    }
    
    if(isKeyPressed(EventKeyboard::KeyCode::KEY_D) || isKeyPressed(EventKeyboard::KeyCode::KEY_KP_RIGHT))
    {
        movementVec += {1.0f, 0.0f};
    }
    
    if(isKeyPressed(EventKeyboard::KeyCode::KEY_W) || isKeyPressed(EventKeyboard::KeyCode::KEY_KP_UP))
    {
        movementVec += {0.0f, 1.0f};
    }
    
    if(isKeyPressed(EventKeyboard::KeyCode::KEY_S) || isKeyPressed(EventKeyboard::KeyCode::KEY_KP_DOWN))
    {
        movementVec += {0.0f, -1.0f};
    }
    
    if(!movementVec.isZero())
    {
        static int nextFrameID = 0;
        
        const Vec2 dir = movementVec * speed * _delta;
        const Vec2 nextPos = getPosition() + dir;
        
        m_framesToSend.emplace_back(++nextFrameID, dir, getPosition());
        setPosition(nextPos);
    }
}

LocalPlayer::Frames&& LocalPlayer::getFrames()
{
    if(!m_framesToSend.empty())
    {
        m_unackFrames.insert(m_unackFrames.end(), m_framesToSend.begin(), m_framesToSend.end());
    }

    return std::move(m_framesToSend);
}

void LocalPlayer::onKeyPressed(EventKeyboard::KeyCode _code, Event* _event)
{
    setKeyPressed(_code, true);
}

void LocalPlayer::onKeyReleased(EventKeyboard::KeyCode _code, Event* _event)
{
    setKeyPressed(_code, false);
}

void LocalPlayer::setKeyPressed(cocos2d::EventKeyboard::KeyCode _code, bool _state)
{
    m_keys[_code] = _state;
}

bool LocalPlayer::isKeyPressed(cocos2d::EventKeyboard::KeyCode _code) const
{
    if(!m_keys.count(_code))
    {
        return false;
    }
    
    return m_keys.at(_code);
}

// RemotePlayer .........

Opponent::Opponent(std::int32_t _id)
: m_id(_id)
{
   
}

int Opponent::getID() const
{
    return m_id;
}

void Opponent::initFromFrame(const ServerFrame& _frame)
{
    setPosition(_frame.pos);
}

void Opponent::pushFrames(Frames&& _frames)
{
    m_unprocessedFrames.insert(m_unprocessedFrames.end(), _frames.begin(), _frames.end());
}

bool Opponent::init()
{
    if(!Parent::init())
    {
        return false;
    }
    
    setTexture("64_white.png");
    setColor(Color3B::GRAY);
    scheduleUpdate();
    
    return true;
}

void Opponent::update(float _delta)
{
    Parent::update(_delta);
    
    if(!m_unprocessedFrames.empty())
    {
        ServerFrame frame = *m_unprocessedFrames.begin();
        m_unprocessedFrames.erase(m_unprocessedFrames.begin());
        
        setPosition(frame.pos);
    }
}
