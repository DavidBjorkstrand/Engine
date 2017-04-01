#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out vec3 localPos;

uniform mat4 proj;
uniform mat4 view;

void main()
{
    localPos = position*2;  
    gl_Position =  (proj * view * vec4(localPos, 1.0)).xyww;
}