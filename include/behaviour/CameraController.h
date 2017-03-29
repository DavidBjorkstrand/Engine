#pragma once

#include "engine/scene/entity/component/Behaviour.h"

class Transform;

class CameraController : public Behaviour
{
	public:
		void update();
};