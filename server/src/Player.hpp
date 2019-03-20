#pragma once

#include <vector>
#include <algorithm>
#include <set>

#include "CommonTypes.hpp"
#include "ReadStream.hpp"
#include "Callback.hpp"

class Player
{
public:
    Player(const common::ID& _id)
        : m_id(_id)
    {

    }

	common::ID getID() const
	{
		return m_id;
	}

public:
	Callback<void()> onDisconnected;
	Callback<void(ReadStream)> onPacketReceived;

private:
    const common::ID m_id;
};
