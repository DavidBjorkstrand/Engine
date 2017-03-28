#pragma once

#include <glm\vec3.hpp>

class Transform
{
    public:
        /*
        * Constructor, created with position [0 0 0]. 
        */
        Transform();

    private: 
        glm::vec3 _position;
};