#pragma once

class Engine;
class InputManager;

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

		/*
		* Gets how much the mouse has moved along x-axis since last frame.
		*/
		static double getDeltaMouseX();

		/*
		* Gets how much the mouse has moved along y-axis since last frame.
		*/
		static double getDeltaMouseY();

    protected:
        /* 
        * Initiates the interface. 
        * param: inputManager - The class that holds the state of the inputs
        */
        static void init(InputManager *inputManager);
    
    private:
        static InputManager *_inputManager;

};