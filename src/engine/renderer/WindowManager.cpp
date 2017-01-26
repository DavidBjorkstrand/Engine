#include "WindowManager.h"
#include "exceptions\GLFWInitException.h"
#include "exceptions\GLEWInitException.h"

#include <iostream>

#define GLEW_STATIC
#include <GL\glew.h>
#include <GLFW\glfw3.h>


void WindowManager::init()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Engine", nullptr, nullptr);
    if (window == nullptr)
    {
        WindowManager::terminate();
        throw glfwInitException;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        WindowManager::terminate();
        throw glewInitException;
    }
}

void WindowManager::terminate() 
{
    glfwTerminate();
}