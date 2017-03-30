#include "engine/InputSystem.h"

#define GLEW_STATIC
#include <GL\glew.h>
#include <GLFW\glfw3.h>

InputSystem::InputSystem()
{
    for (int i = 0; i < 1024; i++)
    {
        _keys[i] = false;
    }

	mouseInitialized = false;

	deltaMousePos[0] = 0;
	deltaMousePos[1] = 0;
}

InputSystem::~InputSystem()
{

}

void InputSystem::setKey(int key, int action)
{
    if (action == GLFW_PRESS) 
    {
        _keys[key] = true;
    } 
    else if (action == GLFW_RELEASE)
    {
        _keys[key] = false;
    }
}

void InputSystem::setMousePos(double xpos, double ypos)
{
	if (!mouseInitialized)
	{
		prevMousePos[0] = xpos;
		prevMousePos[1] = ypos;

		mouseInitialized = true;
	}
	else
	{
		deltaMousePos[0] = prevMousePos[0] - xpos;
		deltaMousePos[1] = prevMousePos[1] - ypos;

		prevMousePos[0] = xpos;
		prevMousePos[1] = ypos;
	}
}

bool InputSystem::checkKey(int key)
{
    return _keys[key];
}

double InputSystem::getDeltaMouseX()
{
	return deltaMousePos[0];
}

double InputSystem::getDeltaMouseY()
{
	return deltaMousePos[1];
}

void InputSystem::pollEvents()
{
	deltaMousePos[0] = 0;
	deltaMousePos[1] = 0;

    glfwPollEvents();
}