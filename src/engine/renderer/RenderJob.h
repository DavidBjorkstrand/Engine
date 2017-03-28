#pragma once

#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

/*
* This class holds information for object the renderer should render.
*/
class RenderJob
{
	public:
		/*
		* Constructor. 
		* glm::mat4 modelMatrix - A model matrix.
		* GLuint VAO - A vertex array object.
		* GLuint nINdices - Amount of indices to render. 
		*/
		RenderJob(glm::mat4 modelMatrix, GLuint VAO, GLuint nIndices);

		/*
		* Deconstructor.
		*/
		~RenderJob();

		/*
		* Gets the model matrix. 
		*/
		glm::mat4 getModelMatrix();

		/*
		* Draws the render job. 
		*/
		void draw();

	private:
		glm::mat4 _modelMatrix;
		GLuint _VAO;
		GLuint _nIndices;
};