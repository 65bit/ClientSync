#pragma once

#include <vector>

class Level
{
public:
	struct Obstacle
	{
		float x{ 0.0f };
		float y{ 0.0f };
	};

	struct StaticGeometry
	{
		float width{ 0.0f };
		float height{ 0.0f };

		std::vector<Obstacle> obstacles;
	};
public:
	void init(const StaticGeometry& _staticGeometry)
	{
		m_geometry = _staticGeometry;
	}

private:
	StaticGeometry m_geometry;
};