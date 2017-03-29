#include "PointLight.h"
#include "..\..\SceneParser.h"

#include <glm/gtc/type_ptr.hpp>

PointLight::PointLight(glm::vec3 color)
{
	_color = color;
}

void PointLight::accept(SceneParser *sceneParser)
{
	sceneParser->visit(this);
}

glm::vec3 PointLight::getColor()
{
	return _color;
}