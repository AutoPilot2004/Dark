#include "Components.h"

namespace Dark
{
	namespace
	{
		constexpr Color s_bgColor = { 0.5f, 0.5f, 0.5f, 1.0f };
		const b2Vec2 s_gravity = { 0.0f, -9.81f };

		constexpr int s_idCounter = 0;

		constexpr int s_activeCamera = -1;
	}

	namespace Components
	{
		void resetStaticVars()
		{
			Environment::backgroundColor = s_bgColor;
			Environment::gravity = s_gravity;

			IdentifierComponent::idCounter = s_idCounter;

			CameraComponent::active = s_activeCamera;
		}
	}

	Color Environment::backgroundColor = s_bgColor;
	b2Vec2 Environment::gravity = s_gravity;

	int IdentifierComponent::idCounter = s_idCounter;

	int CameraComponent::active = s_activeCamera;
}