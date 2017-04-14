#pragma once

#include "Component.h"

class Scene;

class Behaviour : public Component
{
	public:
		void accept(Scene *scene);
		virtual void update(float dt) = 0;
};