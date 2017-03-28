#include "Input.h"
#include "..\InputManager.h"

InputManager *Input::_inputManager = nullptr;

bool Input::checkKey(int key)
{
    return _inputManager->checkKey(key);
}

double Input::getDeltaMouseX()
{
	return _inputManager->getDeltaMouseX();
}

double Input::getDeltaMouseY()
{
	return _inputManager->getDeltaMouseY();
}

void Input::init(InputManager *inputManager)
{
    _inputManager = inputManager;
}