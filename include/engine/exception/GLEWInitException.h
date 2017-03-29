#pragma once

#include<exception>

using namespace std;

/* Exception throwed by WindowManager::init if GLEW initation fails. */
class GLEWInitException : public exception 
{

    virtual const char* what() const throw()
    {
        return "Failed to initilize GLEW";
    }

} glewInitException;