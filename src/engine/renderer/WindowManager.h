#pragma once

/* Class that handles window and other stuff needed to use OpenGL. */
class WindowManager
{
    public:
        /* Initiates OpenGL contex, OpenGL bindings and opens window 
           throws: GLFWInitException, GLEWInitException 
        */
        static void init();

        /* Frees resources used by window */
        static void terminate();
};