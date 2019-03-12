#pragma once

#include "Network.hpp"

class Server
{
	static constexpr unsigned SimulationRate = 16u; // 60hz
	static constexpr unsigned BroadcastRate = 100u; // 10hz

public:
	bool init()
	{
		const bool result = m_network.init();

		if (result)
		{
			m_network.onPlayerConnected.add(this, &Server::onPlayerConnected);
			m_network.onPlayerDisconnected.add(this, &Server::onPlayerDisconnected);
		}

		return result;
	}

	void run()
	{
		auto simulateTime = m_network.getTime();
		auto broadcastTime = m_network.getTime();

		while (true)
		{
			if ((m_network.getTime() - simulateTime) >= SimulationRate)
			{
				if (!m_network.poll(5))
				{
					break;
				}

				simulate();
				simulateTime = m_network.getTime();
			}

			if ((m_network.getTime() - broadcastTime) >= BroadcastRate)
			{
				broadcast();
				broadcastTime = m_network.getTime();
			}
		}
	}

private:
	void simulate()
	{

	}

	void broadcast()
	{

	}

private:
	void onPlayerConnected(Player::ID _id)
	{

	}

	void onPlayerDisconnected(Player::ID _id)
	{

	}

private:
	Network m_network;
};