#version 330 core

in vec2 TexCoords;
in vec3 EyeSpacePos;

out vec3 FragColor;

uniform vec3 eye;
uniform mat4 proj;
uniform sampler2D sprite;

void main()
{	
	vec3 N;
	N.xy = TexCoords*2.0f - 1.0f;
	float r2 = dot(N.xy, N.xy);

	if (r2 > 1.0f)
	{
		discard;
	}

	N.z = -sqrt(1.0f - r2);

	vec4 pixelPos = vec4(eye + N*0.25f, 1.0f);
	vec4 clipSpacePos = proj * pixelPos;

	float zndc = clipSpacePos.z / clipSpacePos.w;
	gl_FragDepth = 0.5f*zndc + 0.5f;

	FragColor = vec3(1.0f, 0.0f, 0.0f);
}