#include "Input.h"
#include "..\InputManager.h"

InputManager *Input::_inputManager = nullptr;

bool Input::checkKey(int key)
{
    return _inputManager->checkKey(key);
}

void Input::init(InputManager *inputManager)
{
    _inputManager = inputManager;
}