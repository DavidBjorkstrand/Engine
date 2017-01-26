#include "InputManager.h"

#define GLEW_STATIC
#include <GL\glew.h>
#include <GLFW\glfw3.h>

InputManager::InputManager() 
{
    for (int i = 0; i < 1024; i++)
    {
        _keys[i] = false;
    }
}

InputManager::~InputManager() 
{

}

void InputManager::setKey(int key, int action) 
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

bool InputManager::checkKey(int key) 
{
    return _keys[key];
}

void InputManager::pollEvents()
{
    glfwPollEvents();
}