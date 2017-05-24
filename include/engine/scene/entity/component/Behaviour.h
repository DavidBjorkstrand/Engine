#pragma once

#include "Component.h"

class Scene;

class Behaviour : public Component
{
	public:
		void accept(Scene *scene);
		void onAttach();
		virtual void update(float dt) = 0;
};