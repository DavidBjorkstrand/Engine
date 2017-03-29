#pragma once

#include "Component.h"

#include <glm/gtc/type_ptr.hpp>

class SceneParser;

/*
* Camera class. 
*/
class Camera : public Component
{
	public:
		/*
		* Constroctor for perspective projection.
		*/
		Camera(float fov, float zNear, float zFar);

		/*
		* Used when parsing scene. 
		*/
		void accept(SceneParser *sceneParser);

		/*
		* Get projection matrix.
		*/
		glm::mat4 getProjectionMatrix();

		/*
		* Gets the view matrix of the camera. 
		*/
		glm::mat4 getViewMatrix();

	private:
		float _fov;
		float _zNear;
		float _zFar;
};