#pragma once

#include "Core.hpp"

class Gun
: public cocos2d::Node
{
    using Parent = cocos2d::Node;
    
public:
    GENERIC_CREATE_FUNC(Gun)
    
private:
    bool init() override;
    void update(float _delta) override;
};
