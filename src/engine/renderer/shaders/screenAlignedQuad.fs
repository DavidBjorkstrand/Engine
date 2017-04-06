#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D image;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; 
    return (2.0 * 0.1f * 100.0f) / (100.0f + 0.1f - z * (100.0f - 0.1f));
}

void main()
{
	float depthValue = texture(image, TexCoords).r;
	FragColor = vec4(vec3(LinearizeDepth(depthValue) / 100.0f), 1.0f);
}