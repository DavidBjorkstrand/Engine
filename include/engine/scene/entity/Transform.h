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

		void setScale(glm::vec3 scale);

		/*
		* Moves the transform
		*/
		void move(glm::vec3 deltaPosition);

		/*
		* Rotates the transform
		*/
		void rotate(float angle, glm::vec3 axis);

		/*
		* Gets the position of the transform in the world. 
		*/
		glm::vec3 getWorldPosition();

		/*
		* Gets the orientation of the transform in the world.
		*/
		glm::quat getWorldOrientation();

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
		glm::vec3 _scale;
		glm::mat4 _parentTransform;
		glm::mat4 _modelMatrix;
		bool _dirty;
};