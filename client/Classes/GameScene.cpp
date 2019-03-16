#include "GameScene.hpp"
#include "ConnectScene.hpp"

using namespace cocos2d;

GameScene::GameScene(Connection&& _connection)
: m_connection(std::move(_connection))
, m_localPlayer(nullptr)
, m_disconnectButton(nullptr)
, m_networkTicks(0.0f)
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
    scheduleUpdate();
    subscribe();
    
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
        stream << static_cast<std::int32_t>(frames.size());
        
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
    std::int32_t opponentsCount = 0;
    _stream >> opponentsCount;
    
    std::vector<int> opponents;
    opponents.reserve(opponentsCount);
    
    while(opponentsCount != 0)
    {
        std::int32_t opponentID = 0;
        
        _stream >> opponentID;
        opponents.push_back(opponentID);
        
        --opponentsCount;
    }
    
    checkOpponents(opponents);
    
    std::int32_t hasLocalFrame = 0;
    _stream >> hasLocalFrame;
    
    if(hasLocalFrame != 0)
    {
        ServerFrame frame;
        _stream >> frame.id >> frame.pos.x >> frame.pos.y;
        m_localPlayer->reconcile(frame);
    }
    
    std::int32_t remotePlayersCount = 0;
    _stream >> remotePlayersCount;
    
    if(remotePlayersCount != 0)
    {
        while(remotePlayersCount != 0)
        {
            int32_t id = 0;
            int32_t framesCount = 0;
            
            _stream >> id >> framesCount;
            
            std::vector<ServerFrame> remoteFrames;
            remoteFrames.reserve(framesCount);
            
            while(framesCount != 0)
            {
                ServerFrame frame;
                _stream >> frame.id >> frame.pos.x >> frame.pos.y;
                remoteFrames.push_back(frame);
                
                --framesCount;
            }
            
            pushOpponentFrames(id, std::move(remoteFrames));
            --remotePlayersCount;
        }
    }
}

void GameScene::checkOpponents(const std::vector<int>& _id)
{
    //Check disconnected
    {
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
    
    // Check connected
    
    for(int id : _id)
    {
        auto it = std::find_if(m_opponents.begin(), m_opponents.end(), [id](Opponent* _opponent)
                               {
                                   return _opponent->getID() == id;
                               });
        
        if(it == m_opponents.end())
        {
            auto opponent = Opponent::create(id);
            addChild(opponent);
            m_opponents.push_back(opponent);
        }
    }
    
}

void GameScene::pushOpponentFrames(int _id, std::vector<ServerFrame>&& _frames)
{
    auto it = std::find_if(m_opponents.begin(), m_opponents.end(), [_id](Opponent* _opponent)
                           {
                               return _opponent->getID() == _id;
                           });
    
    Opponent* opponent = *it;
    opponent->pushFrames(std::move(_frames));
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

Opponent::Opponent(int _id)
: m_id(_id)
{
   
}

int Opponent::getID() const
{
    return m_id;
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
