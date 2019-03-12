#pragma once

struct Vec2
{
	float x{ 0.0f };
	float y{ 0.0f };
};


class Player
{
public:
	struct ID
	{
		int value{ 0 };
	};

public:
	Player(const ID& _id)
		: m_id(_id)
	{

	}

	ID getID() const
	{
		return m_id;
	}

private:
	const ID m_id;
};