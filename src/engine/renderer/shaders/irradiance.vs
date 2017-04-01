#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out vec3 WorldPos;

uniform mat4 proj;
uniform mat4 view;

void main()
{
    vec4 pos = proj * view * vec4(position*2, 1.0);
	gl_Position =  pos.xyww;

	WorldPos = position*2;  
}