#include "engine/WindowSystem.h"

#include "engine/InputSystem.h"
#include "engine/exception/GLFWInitException.h"
#include "engine/exception/GLEWInitException.h"

#include <iostream>

#define GLEW_STATIC
#include <GL\glew.h>
#include <GLFW\glfw3.h>

WindowSystem *WindowSystem::instance = nullptr;

void WindowSystem::show()
{
    glfwSwapBuffers(_window);
}

int WindowSystem::getWindowHeight()
{
	int width;
	int height;

	glfwGetWindowSize(_window, &width, &height);

	return height;
}

int WindowSystem::getWindowWidth()
{
	int width;
	int height;

	glfwGetWindowSize(_window, &width, &height);

	return width;

}

WindowSystem::WindowSystem(InputSystem *inputSystem)
{
    if (instance != nullptr) {
        delete instance;
    }

    _inputSystem = inputSystem;
    init();

    instance = this;
}

WindowSystem::~WindowSystem()
{
    terminate();
}

void WindowSystem::init()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);

    _window = glfwCreateWindow(1280, 720, "Engine", nullptr, nullptr);
    if (_window == nullptr)
    {
        terminate();
        throw glfwInitException;
    }

    glfwMakeContextCurrent(_window);

	glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetKeyCallback(_window, keyCallback);

	glfwSetCursorPosCallback(_window, mousePosCallBack);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        terminate();
        throw glewInitException;
    }
}

void WindowSystem::terminate()
{
    glfwTerminate();
}

void WindowSystem::keyCallback(GLFWwindow *window, int key, int scancode,
    int action, int mode)
{
    WindowSystem::instance->_inputSystem->setKey(key, action);
}

void WindowSystem::mousePosCallBack(GLFWwindow *window, double xpos, double ypos)
{
	WindowSystem::instance->_inputSystem->setMousePos(xpos, ypos);
}