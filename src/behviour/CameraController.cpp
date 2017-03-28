#include "CameraController.h"
#include "..\engine\interfaces\Input.h"
#include "..\engine\scene\entity\Entity.h"
#include "..\engine\scene\entity\Transform.h"

#include <GLFW\glfw3.h>

void CameraController::update()
{
	Transform *transform = getEntity()->getTransform();
	glm::vec3 right = glm::cross(transform->getWorldDirection(), glm::vec3(0.0f, 1.0f, 0.0f));

	if (Input::checkKey(GLFW_KEY_W))
	{
		transform->move(0.1f * transform->getWorldDirection());
	}

	if (Input::checkKey(GLFW_KEY_S))
	{
		transform->move(-0.1f * transform->getWorldDirection());
	}

	if (Input::checkKey(GLFW_KEY_D))
	{
		transform->move(0.1f * right);
	}

	if (Input::checkKey(GLFW_KEY_A))
	{
		transform->move(-0.1f * right);
	}

	double deltaX = Input::getDeltaMouseX();

	transform->rotate((float) deltaX/10.0f, glm::vec3(0.0f, 1.0f, 0.0f));

	double deltaY = Input::getDeltaMouseY();

	transform->rotate((float)deltaY / 10.0f, glm::vec3(1.0f, 0.0f, 0.0f));

}