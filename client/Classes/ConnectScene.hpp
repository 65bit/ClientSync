#pragma once

#include "Core.hpp"
#include "Connection.hpp"

class ConnectScene final
    : public cocos2d::Scene
{
    using Parent = cocos2d::Scene;

public:
    ConnectScene();
    ConnectScene(Connection&& _connection);
    ~ConnectScene();
    
    GENERIC_CREATE_FUNC(ConnectScene)
    
private:
    bool init() override;
    void update(float _delta) override;
    
    void subscribe();
    void unsubscribe();
    void updateState();
    
    void onConnectionStateChanged(Connection::State _newState);
    void onButtonTouch(cocos2d::Ref* _sender, cocos2d::ui::Widget::TouchEventType _type);
    
private:
    Connection m_connection;
    cocos2d::ui::Button* m_connectButton;
};
