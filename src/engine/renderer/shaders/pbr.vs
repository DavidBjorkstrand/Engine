#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 Position;
out vec3 Normal;
out vec2 TexCoords;

void main()
{
	gl_PointSize = 10.0f;
	gl_Position = proj * view * model * vec4(position, 1.0f);

	Position = vec3(model * vec4(position, 1.0f));
	Normal = mat3(transpose(inverse(model))) * normal;
	TexCoords = vec2(texCoords.x, 1.0f - texCoords.y);
}