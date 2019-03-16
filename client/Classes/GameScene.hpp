#pragma once

#include "Core.hpp"
#include "Connection.hpp"

struct LocalFrame
{
    std::int32_t id{0};
    
    cocos2d::Vec2 dir;
    cocos2d::Vec2 oldPos;

    LocalFrame()
    {
        
    }
    
    LocalFrame(int _id, const cocos2d::Vec2& _dir, const cocos2d::Vec2& _oldPos)
    : id(_id)
    , dir(_dir)
    , oldPos(_oldPos)
    {
        
    }
};

struct ServerFrame
{
    std::int32_t id{0};
    cocos2d::Vec2 pos;
};

class LocalPlayer
: public cocos2d::Sprite
{
    using Parent = cocos2d::Sprite;
    using Frames = std::vector<LocalFrame>;
    
public:
    GENERIC_CREATE_FUNC(LocalPlayer)
    
public:
    LocalPlayer();
    
    void reconcile(const ServerFrame& _frame);
    Frames&& getFrames();
    
private:
    bool init() override;
    void update(float _delta) override;
    
    void onKeyPressed(cocos2d::EventKeyboard::KeyCode _code, cocos2d::Event* _event);
    void onKeyReleased(cocos2d::EventKeyboard::KeyCode _code, cocos2d::Event* _event);
    
    void setKeyPressed(cocos2d::EventKeyboard::KeyCode _code, bool _state);
    bool isKeyPressed(cocos2d::EventKeyboard::KeyCode _code) const;
    
private:
    cocos2d::EventListenerKeyboard* m_keyboardListener;
    
    Frames m_framesToSend;
    Frames m_unackFrames;
    
    std::map<cocos2d::EventKeyboard::KeyCode, bool> m_keys;
};

class Opponent
: public cocos2d::Sprite
{
    using Parent = cocos2d::Sprite;
    using Frames = std::vector<ServerFrame>;
    
public:
    Opponent(int _id);
    GENERIC_CREATE_FUNC(Opponent)
    
    int getID() const;
    void pushFrames(Frames&& _frames);
    
private:
    bool init() override;
    void update(float _delta) override;
    
private:
    const int m_id;
    Frames m_unprocessedFrames;
};

class GameScene
    : public cocos2d::Scene
{
    using Parent = cocos2d::Scene;

public:
    GameScene(Connection&& _connection);
    ~GameScene();
    
    GENERIC_CREATE_FUNC(GameScene);

private:
    bool init() override;
    void update(float _delta) override;
    
    void returnToConnectScene();
    void checkOpponents(const std::vector<int>& _id);
    void pushOpponentFrames(int _id, std::vector<ServerFrame>&& _frames);
    void createLocalPlayer();
    void subscribe();
    void unsubscribe();
    
    void onButtonTouch(cocos2d::Ref* _sender, cocos2d::ui::Widget::TouchEventType _type);
    void onConnectionStateChanged(Connection::State _newState);
    void onPacketReceived(ReadStream _stream);
    
private:
    LocalPlayer* m_localPlayer;
    
    std::vector<Opponent*> m_opponents;
    
    cocos2d::ui::Button* m_disconnectButton;

    Connection m_connection;
    
    float m_networkTicks;
};
