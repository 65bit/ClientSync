#pragma once

#include <cocos2d.h>
#include <CCPlatformMacros.h>
#include <ui/CocosGUI.h>

#include <functional>
#include <vector>
#include <map>
#include <type_traits>
#include <memory>
#include <string>

#include "Callback.hpp"

#define DIRECTOR cocos2d::Director::getInstance()
#define VIEW cocos2d::Director::getInstance()->getOpenGLView()

#define GENERIC_CREATE_FUNC(__TYPE__) \
template<typename... T> \
static __TYPE__* create(T&&... _args) \
{ \
    __TYPE__* result = new __TYPE__(std::forward<T>(_args)...); \
    \
    if(result->init()) \
    { \
        result->autorelease(); \
    } \
    else \
    { \
        CC_SAFE_DELETE(result); \
    } \
    \
    return result; \
}
