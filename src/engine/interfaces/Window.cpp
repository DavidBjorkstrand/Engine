#include "Window.h"

WindowManager *Window::_windowManager = nullptr;

int Window::getWindowHeight()
{
	return _windowManager->getWindowHeight();
}

int Window::getWindowWidth()
{
	return _windowManager->getWindowWidth();
}

void Window::init(WindowManager *windowManager)
{
	_windowManager = windowManager;
}