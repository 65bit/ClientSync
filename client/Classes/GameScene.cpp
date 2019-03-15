#include "GameScene.hpp"
#include "ConnectScene.hpp"

using namespace cocos2d;

GameScene::GameScene(Connection&& _connection)
: m_connection(std::move(_connection))
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
    
    return true;
}

void GameScene::update(float _delta)
{
    Parent::update(_delta);
    m_connection.poll();
}

void GameScene::onButtonTouch(cocos2d::Ref* _sender, cocos2d::ui::Widget::TouchEventType _type)
{
    if(_type != ui::Widget::TouchEventType::ENDED)
    {
        return;
    }
    
    if(_sender == m_disconnectButton)
    {
        m_connection.disconnect();
        DIRECTOR->replaceScene(ConnectScene::create(std::move(m_connection)));
    }
    else
    {
        CCLOG("Unknown button");
    }
}
