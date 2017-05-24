#pragma once

#include "engine/physics/Rigidbody.h"
#include "engine/scene/entity/component/Component.h"

#include <glm/gtc/type_ptr.hpp>

class Scene;

class RigidbodyComponent : public RigidbodyListener, public Component
{
	private:
		Rigidbody *_rigidbody;

	public:
		RigidbodyComponent(float mass);
		void accept(Scene *scene);
		void onAttach();
		void notify(glm::vec3 position);
		void notify(glm::quat rotation);

		Rigidbody *getRigidbody();
};