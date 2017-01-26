#pragma once

#include "..\InputManager.h"

class Engine;

class Input
{
    friend class Engine;

    public:
        static bool checkKey(int key);

    protected:
        static void init(InputManager *inputManager);
    
    private:
        static InputManager *_inputManager;

};