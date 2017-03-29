#pragma once

#include<exception>

using namespace std;

/* Exception throwed by WindowManager::init if GLFW initation fails. */
class GLFWInitException : public exception 
{
    
    virtual const char* what() const throw() 
    {
        return "Failed to initilize GLFW";
    }

} glfwInitException;