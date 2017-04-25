#include "behaviour/CameraController.h"

#include "engine/interface/Input.h"
#include "engine/scene/entity/Entity.h"
#include "engine/scene/entity/Transform.h"

#include <GLFW\glfw3.h>

void CameraController::update(float dt)
{
	Transform *transform = getEntity()->getTransform();
	glm::vec3 direction = transform->getWorldDirection();
	glm::vec3 right = glm::normalize(glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f)));
	glm::vec3 up = glm::normalize(glm::cross(right, direction));

	if (Input::checkKey(GLFW_KEY_W))
	{
		transform->move(dt * 10.0f * transform->getWorldDirection());
	}

	if (Input::checkKey(GLFW_KEY_S))
	{
		transform->move(-dt * 10.0f * transform->getWorldDirection());
	}

	if (Input::checkKey(GLFW_KEY_D))
	{
		transform->move(dt * 10.0f * right);
	}

	if (Input::checkKey(GLFW_KEY_A))
	{
		transform->move(-dt * 10.0f * right);
	}

	double deltaX = Input::getDeltaMouseX();

	transform->rotate((float) deltaX*dt*2.0f, up);

	double deltaY = Input::getDeltaMouseY();

	transform->rotate((float)deltaY*dt*2.0f, right);

}