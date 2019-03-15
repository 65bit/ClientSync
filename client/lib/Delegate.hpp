#pragma once

#include "FastDelegate.hpp"

template<typename T>
class Delegate : public fastdelegate::FastDelegate<T> {};