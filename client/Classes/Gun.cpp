#include "Gun.hpp"

using namespace cocos2d;

bool Gun::init()
{
    if(!Parent::init())
    {
        return false;
    }
    
    Sprite* platform = Sprite::create("64_white.png");
    platform->setScale(0.5f);
    addChild(platform);
    
    Sprite* muzzle = Sprite::create("64_white.png");
    muzzle->setScale(0.3f, 0.7f);
    muzzle->setAnchorPoint({0.5f, 0.1f});
    muzzle->setColor(Color3B::GREEN);
    muzzle->runAction(RepeatForever::create(RotateBy::create(1.0f, 360.0f)));
    addChild(muzzle);
    
    return true;
}

void Gun::update(float _delta)
{
    Parent::update(_delta);
}
