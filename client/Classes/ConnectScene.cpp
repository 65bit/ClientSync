#include "ConnectScene.hpp"
#include "GameScene.hpp"

using namespace cocos2d;

ConnectScene::ConnectScene()
{
    
}

ConnectScene::ConnectScene(Connection&& _connection)
: m_connection(std::move(_connection))
{
    
}

ConnectScene::~ConnectScene()
{
    unsubscribe();
}

bool ConnectScene::init()
{
    if(!Parent::init())
    {
        return false;
    }
    
    const auto designSize = VIEW->getDesignResolutionSize();
    
    m_connectButton = ui::Button::create("64_white.png");
    m_connectButton->setScale(2.0f);
    m_connectButton->setPosition(designSize * 0.5f);
    m_connectButton->setTitleFontSize(35.0f);
    m_connectButton->setTitleColor(Color3B::RED);
    m_connectButton->addTouchEventListener(CC_CALLBACK_2(ConnectScene::onButtonTouch, this));
    addChild(m_connectButton);
    
    updateState();
    scheduleUpdate();
    subscribe();
    
    return true;
}

void ConnectScene::update(float _delta)
{
    Parent::update(_delta);
    m_connection.poll();
}

void ConnectScene::subscribe()
{
    m_connection.onStateChanged.add(this, &ConnectScene::onConnectionStateChanged);
}

void ConnectScene::unsubscribe()
{
    m_connection.onStateChanged.remove(this, &ConnectScene::onConnectionStateChanged);
}

void ConnectScene::updateState()
{
    const auto state = m_connection.getState();
    
    m_connectButton->setTitleText(Connection::ToString(state));
    m_connectButton->setEnabled(state == Connection::State::Undefined ||
                                state == Connection::State::Initialized ||
                                state == Connection::State::Disconnected);
}

void ConnectScene::onConnectionStateChanged(Connection::State _newState)
{
    updateState();
    
    if(_newState == Connection::State::Connected)
    {
        unsubscribe();
        DIRECTOR->replaceScene(GameScene::create(std::move(m_connection)));
    }
}

void ConnectScene::onButtonTouch(cocos2d::Ref* _sender, cocos2d::ui::Widget::TouchEventType _type)
{
    if(_type != ui::Widget::TouchEventType::ENDED)
    {
        return;
    }
    
    m_connection.connect();
}
