#include "engine/scene/entity/Transform.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

Transform::Transform()
{
    _position.x = 0;
    _position.y = 0;
    _position.z = 0;
}

void Transform::setPosition(glm::vec3 position)
{
	_position = position;
}

void Transform::move(glm::vec3 deltaPosition)
{
	_position += deltaPosition;
}

void Transform::rotate(float angle, glm::vec3 axis)
{
	glm::quat rotation = glm::normalize(glm::angleAxis(glm::radians(angle), axis));

	_orientation *= rotation;
}

glm::vec3 Transform::getWorldPosition()
{
	return glm::vec3(_parentTransform * glm::vec4(_position, 1.0f));
}

glm::quat Transform::getWorldOrientation()
{
	return _orientation * glm::quat(glm::mat3(_parentTransform));
}

glm::vec3 Transform::getWorldDirection()
{
	return glm::mat3(getMatrix()) * glm::vec3(0.0f, 0.0f, -1.0f);
}

void Transform::setParentTransform(glm::mat4 parentTransform)
{
	_parentTransform = parentTransform;
}

glm::mat4 Transform::getMatrix()
{
	glm::mat4 translate = glm::translate(glm::mat4(), _position);
	glm::mat4 rotate = glm::toMat4(_orientation);

	return _parentTransform * translate * rotate;
}