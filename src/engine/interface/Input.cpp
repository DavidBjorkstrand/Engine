#include "engine/interface/Input.h"

#include "engine/InputSystem.h"


InputSystem *Input::_inputSystem = nullptr;

bool Input::checkKey(int key)
{
    return _inputSystem->checkKey(key);
}

double Input::getDeltaMouseX()
{
	return _inputSystem->getDeltaMouseX();
}

double Input::getDeltaMouseY()
{
	return _inputSystem->getDeltaMouseY();
}

void Input::init(InputSystem *inputManager)
{
    _inputSystem = inputManager;
}