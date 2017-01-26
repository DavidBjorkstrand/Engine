#pragma once

class Engine;

/* This class keeps track of inputs */
class InputManager 
{
    friend class Engine;

    public:
        /* 
        *  Sets the state of a key depending on action to be 
        *  pressed or not pressed. Gets called from GLFW keycallback in 
        *  WindowManager class. 
        *  param: key - The key in question.
        *  param: action - If the key was pressed or released. 
        */
        void setKey(int key, int action);

        /*
        * Checks if a key is pressed or not.
        * param: key - The key to check. 
        * return: True if the key is pressed else false.
        */
        bool checkKey(int key);

        /*
        * Polls events, needs to be called each frame.  
        */
        void pollEvents();

    protected:
        /*
        * Constuctor for InputManager. Should only be called from Engine class.
        */
        InputManager();

        /*
        * Deconstructor for InputManager. Should only be called from 
        * Engine class.
        */
        ~InputManager();

    private:
        bool _keys[1024];
        
};