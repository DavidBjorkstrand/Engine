#pragma once

#include "Component.h"

class SceneParser;

class Behaviour : public Component
{
	public:
		void accept(SceneParser *sceneParser);
		virtual void update() = 0;
};