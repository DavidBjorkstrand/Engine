#version 330 core

in vec2 TexCoords;
in vec3 EyeSpacePos;

out vec3 FragColor;

uniform mat4 proj;
uniform float radius;

float projectZ(float z)
{
	float A = proj[2].z;
	float B = proj[3].z;

	return 0.5f*(-A*z + B) / z + 0.5f;
}

void main()
{
	vec3 N;
	N.xy = gl_PointCoord*2.0f - 1.0f;
	float r2 = dot(N.xy, N.xy);

	if (r2 > 1.0f)
	{
		discard;
	}

	N.z = sqrt(1.0f - r2);

	vec4 eyeSpacePosition = vec4(EyeSpacePos + N*radius, 1.0f);

	gl_FragDepth = projectZ(abs(eyeSpacePosition.z));
	FragColor = vec3(0.5f, 0.0f, 0.0f);
}