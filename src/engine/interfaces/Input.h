#pragma once

#include "..\InputManager.h"

class Engine;

/* Input interface class used for quering about inputs */
class Input
{
    friend class Engine;

    public:
        /* 
        * Checks if a key is pressed or not. 
        * param: key - The key to check.
        * return: True if key was pressed else false 
        */
        static bool checkKey(int key);

    protected:
        /* 
        * Initiates the interface. 
        * param: inputManager - The class that holds the state of the inputs
        */
        static void init(InputManager *inputManager);
    
    private:
        static InputManager *_inputManager;

};