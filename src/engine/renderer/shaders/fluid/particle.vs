#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

uniform float radius;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec2 TexCoords;
out vec3 EyeSpacePos;

void main()
{
	mat4 modelView = view * model;

	modelView[0][0] = radius*2.0f; 
	modelView[0][1] = 0.0f;
	modelView[0][2] = 0.0f;

	modelView[1][0] = 0.0f;
	modelView[1][1] = radius*2.0f;
	modelView[1][2] = 0.0f;

	modelView[2][0] = 0.0f; 
	modelView[2][1] = 0.0f; 
	modelView[2][2] = radius*2.0f; 

	gl_Position = proj * modelView * vec4(position, 1.0f);

	TexCoords = texCoords;
	TexCoords.y = (1.0f - TexCoords.y);
	EyeSpacePos = vec3(modelView * vec4(position, 1.0f));
}