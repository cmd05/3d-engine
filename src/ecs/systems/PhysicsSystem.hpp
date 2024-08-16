#pragma once

#include <ecs/core/System.hpp>

class PhysicsSystem : public System
{
public:
	void Init();

	void Update(float dt);
};
