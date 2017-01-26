#pragma once

class Engine;
class InputManager;
struct GLFWwindow;

/* Class that handles window and other stuff needed to use OpenGL. */
class WindowManager
{
    friend class Engine;
    
    public:
        void show();

    protected:
        /* 
        * Constructor for the WindowManager can only be one, constuctor
        * deletes previous instance when called. Should only be called from
        * Engine class.
        * throws: GLFWInitException, GLEWInitException 
        */
        WindowManager(InputManager *inputManager);

        /* 
        * Deconstructor for the WindowManager, should only be called from
        * Engine class.
        * throws: GLFWInitException, GLEWInitException 
        */
        ~WindowManager();


    private:
        /* 
        * Initiates OpenGL contex, OpenGL bindings and opens window 
        * throws: GLFWInitException, GLEWInitException 
        */
        void init();

        /* 
        * Frees resources used by window 
        */
        void terminate();

        /*
        * Key callback function sent to GLFW
        */
        static void keyCallback(GLFWwindow *window, int key, int scancode,
            int action, int mode);

        static WindowManager *instance;
        InputManager *_inputManager;
        GLFWwindow *_window;
        
};