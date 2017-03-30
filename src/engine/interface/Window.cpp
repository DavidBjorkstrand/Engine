#include "engine/interface/Window.h"

#include "engine/WindowSystem.h"

WindowSystem *Window::_windowSystem = nullptr;

int Window::getWindowHeight()
{
	return _windowSystem->getWindowHeight();
}

int Window::getWindowWidth()
{
	return _windowSystem->getWindowWidth();
}

void Window::init(WindowSystem *windowSystem)
{
	_windowSystem = windowSystem;
}