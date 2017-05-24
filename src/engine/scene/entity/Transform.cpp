#include "engine/scene/entity/Transform.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

Transform::Transform()
{
    _position.x = 0;
    _position.y = 0;
    _position.z = 0;

	_scale.x = 1;
	_scale.y = 1;
	_scale.z = 1;

	_dirty = false;
}

void Transform::setPosition(glm::vec3 position)
{
	_position = position;
	_dirty = true;
}

void Transform::setScale(glm::vec3 scale)
{
	_scale = scale;
	_dirty = true;
}

void Transform::setOrientation(glm::quat orientation)
{
	_orientation = orientation;
	_dirty = true;
}

void Transform::move(glm::vec3 deltaPosition)
{
	_position += deltaPosition;
	_dirty = true;
}

void Transform::rotate(float angle, glm::vec3 axis)
{
	glm::quat rotation = glm::normalize(glm::angleAxis(glm::radians(angle), axis));

	_orientation *= rotation;
	_dirty = true;
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
	return glm::normalize(glm::mat3(getMatrix()) * glm::vec3(0.0f, 0.0f, -1.0f));
}

void Transform::setParentTransform(glm::mat4 parentTransform)
{
	_parentTransform = parentTransform;
	_dirty = true;
}

glm::mat4 Transform::getMatrix()
{
	if (_dirty)
	{
		glm::mat4 translate = glm::translate(glm::mat4(), _position);
		glm::mat4 rotate = glm::toMat4(_orientation);
		glm::mat4 scale = glm::scale(glm::mat4(), _scale);
		_modelMatrix = _parentTransform * translate * rotate * scale;
		_dirty = false;
	}

	return _modelMatrix;
}