#pragma once
#include <box2d/box2d.h>

namespace Dark
{
	class World
	{
	public:
		World() : m_world({ 0.0f, -9.81f }) {}
		~World() = default;

		inline b2World& get() { return m_world; }

	private:
		b2World m_world;
	};
}