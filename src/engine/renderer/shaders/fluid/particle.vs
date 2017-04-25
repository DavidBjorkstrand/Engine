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
	vec2 viewportSize = vec2(1280.0f, 720.0f);
	mat4 modelView = view * model;

	gl_Position = proj * modelView * vec4(position, 1.0f);
	vec2 centre = (0.5f * gl_Position.xy/gl_Position.w + 0.5f) * viewportSize;
	gl_PointSize = viewportSize.y * proj[1][1] * radius / gl_Position.w;

	TexCoords = texCoords;
	TexCoords.y = (1.0f - TexCoords.y);
	EyeSpacePos = vec3(modelView * vec4(position, 1.0f));
}