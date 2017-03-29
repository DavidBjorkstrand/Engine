#include "Camera.h"
#include "..\..\..\interfaces\Window.h"
#include "..\..\SceneParser.h"
#include "..\Entity.h"
#include "..\Transform.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(float fov, float zNear, float zFar)
{
	_fov = fov;
	_zNear = zNear;
	_zFar = zFar;
}

void Camera::accept(SceneParser *sceneParser)
{
	sceneParser->visit(this);
}

glm::mat4 Camera::getProjectionMatrix()
{
	float aspect = (float)Window::getWindowWidth() / (float)Window::getWindowHeight();

	return glm::perspective(glm::radians(_fov), aspect, _zNear, _zFar);
}

glm::mat4 Camera::getViewMatrix()
{
	glm::vec3 position = getEntity()->getTransform()->getWorldPosition();
	glm::vec3 lookAt = position + getEntity()->getTransform()->getWorldDirection();
	glm::vec3 up = getEntity()->getTransform()->getWorldOrientation() * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f) * glm::conjugate(getEntity()->getTransform()->getWorldOrientation());

	return glm::lookAt(position, lookAt, up);
}