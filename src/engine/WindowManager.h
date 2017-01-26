#pragma once

#include <GLFW\glfw3.h>

class Engine;
class InputManager;

/* Class that handles window and other stuff needed to use OpenGL. */
class WindowManager
{
    friend class Engine;
    
    protected:
        /* Constructor for the WindowManager, calls init() 
           throws: GLFWInitException, GLEWInitException */
        WindowManager(InputManager *inputManager);

        /* Deconstructor for the WindowManager, calls terminate() 
           throws: GLFWInitException, GLEWInitException */
        ~WindowManager();


    private:
        /* Initiates OpenGL contex, OpenGL bindings and opens window 
           throws: GLFWInitException, GLEWInitException 
        */
        void init();

        /* Frees resources used by window */
        void terminate();

        void keyCallback(GLFWwindow *window, int key, int scancode, 
            int action, int mode);

        InputManager *_inputManager;
        GLFWwindow *_window;
        
};