#include "WindowManager.h"
#include "InputManager.h"
#include "exceptions\GLFWInitException.h"
#include "exceptions\GLEWInitException.h"

#include <iostream>

#define GLEW_STATIC
#include <GL\glew.h>
#include <GLFW\glfw3.h>

WindowManager *WindowManager::instance = nullptr;

void WindowManager::show()
{
    glfwSwapBuffers(_window);
}

int WindowManager::getWindowHeight()
{
	int width;
	int height;

	glfwGetWindowSize(_window, &width, &height);

	return height;
}

int WindowManager::getWindowWidth()
{
	int width;
	int height;

	glfwGetWindowSize(_window, &width, &height);

	return width;

}

WindowManager::WindowManager(InputManager *inputManager)
{
    if (instance != nullptr) {
        delete instance;
    }

    _inputManager = inputManager;
    init();

    instance = this;
}

WindowManager::~WindowManager() 
{
    terminate();
}

void WindowManager::init()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

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

void WindowManager::terminate() 
{
    glfwTerminate();
}

void WindowManager::keyCallback(GLFWwindow *window, int key, int scancode,
    int action, int mode)
{
    WindowManager::instance->_inputManager->setKey(key, action);
}

void WindowManager::mousePosCallBack(GLFWwindow *window, double xpos, double ypos)
{
	WindowManager::instance->_inputManager->setMousePos(xpos, ypos);
}