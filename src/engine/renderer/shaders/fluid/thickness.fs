#version 330 core

in vec2 TexCoords;

out vec3 FragColor;

void main()
{
	vec3 N;
	N.xy = TexCoords*2.0f - 1.0f;
	float r2 = dot(N.xy, N.xy);

	if (r2 > 1.0f)
	{
		discard;
	}

	FragColor = vec3(0.5f, 0.0f, 0.0f);
}