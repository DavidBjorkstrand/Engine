#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

uniform mat4 view;
uniform mat4 proj;

out vec3 TexCoords;

void main()
{
	vec4 pos = proj * mat4(mat3(view)) * vec4(position*2, 1.0f);
	gl_Position = pos.xyww;
	TexCoords = position*2;
}