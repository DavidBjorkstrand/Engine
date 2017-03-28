#pragma once

#include <glm/gtc/type_ptr.hpp>

class Transform
{
    public:
        /*
        * Constructor, created with position [0 0 0]. 
        */
        Transform();

		/*
		* Sets the position of the transform
		*/
		void setPosition(glm::vec3 position);

		/*
		* Moves the transform
		*/
		void move(glm::vec3 deltaPosition);

		/*
		* Rotates the transform
		*/
		void rotate(float angle, glm::vec3 axis);

		/*
		* Gets the direction of the transform in world. 
		* Initial direction is always along negative z-axis.
		*/
		glm::vec3 getWorldDirection();

		/*
		* Sets a transform matrix gotten from parent entites. 
		*/
		void setParentTransform(glm::mat4 parentTransform);

		/*
		* Gets a model matrix for the transform. 
		*/
		glm::mat4 getMatrix();

    private: 
        glm::vec3 _position;
		glm::quat _orientation;
		glm::mat4 _parentTransform;
};