#include "InputManager.h"
#include <GLFW\glfw3.h>

InputManager::InputManager() 
{
    
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