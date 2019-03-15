#pragma once

#include "Core.hpp"
#include "Connection.hpp"

class GameScene
    : public cocos2d::Scene
{
    using Parent = cocos2d::Scene;

public:
    GameScene(Connection&& _connection);
    GENERIC_CREATE_FUNC(GameScene);

private:
    bool init() override;
    void update(float _delta) override;
    
    void onButtonTouch(cocos2d::Ref* _sender, cocos2d::ui::Widget::TouchEventType _type);
    
private:
    cocos2d::ui::Button* m_disconnectButton;
    Connection m_connection;
};
