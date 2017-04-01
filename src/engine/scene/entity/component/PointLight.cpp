#include "engine/scene/entity/component/PointLight.h"

#include "engine/scene/Scene.h"

#include <glm/gtc/type_ptr.hpp>

PointLight::PointLight(glm::vec3 color)
{
	_color = color;
}

void PointLight::accept(Scene *scene)
{
	scene->visit(this);
}

glm::vec3 PointLight::getColor()
{
	return _color;
}