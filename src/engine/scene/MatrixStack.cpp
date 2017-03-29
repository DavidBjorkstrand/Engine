#include "engine/scene/MatrixStack.h"

#include <glm/gtc/type_ptr.hpp>

MatrixStack::MatrixStack()
{
	glm::mat4 identity = glm::mat4();

	_stack.push(identity);
}

void MatrixStack::pushMatrix()
{
	glm::mat4 top = _stack.top();
	glm::mat4 copy = glm::mat4(top);

	_stack.push(copy);
}

void MatrixStack::popMatrix()
{
	if (_stack.size() > 1)
	{
		_stack.pop();
	}
}

void MatrixStack::applyMatrix(glm::mat4 transform)
{
	glm::mat4 top = _stack.top();
	glm::mat4 new_top;

	_stack.pop();
	new_top = top * transform;
	_stack.push(new_top);
}

glm::mat4 MatrixStack::top()
{
	return _stack.top();
}