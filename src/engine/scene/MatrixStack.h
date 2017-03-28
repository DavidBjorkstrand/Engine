#pragma once

#include <stack>
#include <glm/gtc/type_ptr.hpp>

/*
* A stack for glm::mat4 matrices. 
*/
class MatrixStack
{
public:

	/*
	* Constructor.
	*/
	MatrixStack();

	/*
	* Copies the top matrix and pushes the copy on the stack.
	*/
	void pushMatrix();

	
	/*
	* Pops the top matrix.
	*/
	void popMatrix();

	/*
	* Multiplies the top matrix with the param matrix. With the 
    * param matrix on the right side.
	*/
	void applyMatrix(glm::mat4 transform);

	/*
	* Returns the matrix at the top of the stack.
	*/
	glm::mat4 top();

private:
	std::stack<glm::mat4> _stack;
};