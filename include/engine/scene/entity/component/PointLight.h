#pragma once

#include "Component.h"

#include <glm/gtc/type_ptr.hpp>

class Scene;

class PointLight : public Component
{
	public:
		PointLight(glm::vec3 color);
		
		/*
		* Used when parsing the scene.
		*/
		void accept(Scene *scene);

		/*
		* Gets the color of the light.
		*/
		glm::vec3 getColor();

	private:
		glm::vec3 _color;
};
