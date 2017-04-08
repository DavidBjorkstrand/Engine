#version 330 core

in vec2 TexCoords;
out vec3 FragColor;

uniform sampler2D depth;
uniform mat4 proj;
uniform float dt;

const float thresehold = 1.0f;

float diffZ(vec2 uv, vec2 offset)
{
	float d = texture(depth, uv).x;
	float dp = texture(depth, uv+offset).x;
	float dm = texture(depth, uv-offset).x;

	if (dm == 0.0f || dp == 0.0f) return 0.0f;
	if (abs(dm - d) > thresehold || abs(dp - d) > thresehold) return 0.0f;

	return (dp - dm) / 2.0f;
}

float diffZ_2(vec2 uv, vec2 offset)
{
	float d = texture(depth, uv).x;
	float dp = texture(depth, uv+offset).x;
	float dm = texture(depth, uv-offset).x;

	if (abs(dm - d) > thresehold || abs(dp - d) > thresehold) return 0.0f;

	return (dp-2.0f*d+dm);
}

float diffZ_xy(vec2 uv, vec2 right, vec2 down)
{
	if (abs(texture(depth, uv+right+down).x - texture(depth, uv).x) > thresehold) return 0.0f;
	if (abs(texture(depth, uv+right-down).x - texture(depth, uv).x) > thresehold) return 0.0f;
	if (abs(texture(depth, uv-right+down).x - texture(depth, uv).x) > thresehold) return 0.0f;
	if (abs(texture(depth, uv-right-down).x - texture(depth, uv).x) > thresehold) return 0.0f;

	return (texture(depth, uv+right+down).x - texture(depth, uv+right-down).x - texture(depth, uv-right+down).x + texture(depth, uv-right-down).x) / 4.0f;
}

float divN(vec2 uv)
{
	vec2 texelSize = 1.0f / textureSize(depth, 0);
	vec2 right = vec2(texelSize.x, 0.0f);
	vec2 down = vec2(0.0f, -texelSize.y);

	float w = proj[0][0];
	float h = proj[1][1];
	float width = 1280;
	float height = 720;

	float z = texture(depth, uv).x;
	float z_x = diffZ(uv, right);
	float z_y = diffZ(uv, down);

	float Cx = -2.0f/(width * w);
	float Cy = -2.0f/(height * h);

	float sx = uv.x;
	float sy = uv.y;
	float Wx = (width-2.0f*sx) / (width * w);
	float Wy = (height-2.0f*sy) / (height * h);

	float D = Cy*Cy*z_x*z_x + Cx*Cx*z_y*z_y+Cx*Cx*Cy*Cy*z*z;
	float z_xx = diffZ_2(uv, right);
	float z_yy = diffZ_2(uv, down);
	float z_xy = diffZ_xy(uv, right, down);
	float D_x = 2*Cy*Cy*z_x*z_xx + 2*Cx*Cx*z_y*z_xy + 2*Cx*Cx*Cy*Cy*z*z_x;
	float D_y = 2*Cy*Cy*z_x*z_xy + 2*Cx*Cx*z_y*z_yy + 2*Cx*Cx*Cy*Cy*z*z_y;
	float Ex = 0.5*z_x*D_x-z_xx*D;
	float Ey = 0.5*z_y*D_y-z_yy*D;
	float H = (Cy*Ex+Cx*Ey)/(2*D*sqrt(D));

	return H;
}

void main()
{
	float depthSample = texture(depth, TexCoords).r;

	if (depthSample == 0.0f)
	{
		FragColor = vec3(0.0f, 0.0f, 0.0f);
	}
	else
	{
		vec2 texelSize = 1.0f / textureSize(depth, 0);
		vec2 right = vec2(texelSize.x, 0.0f);
		vec2 down = vec2(0.0f, texelSize.y);

		float z_x = diffZ(TexCoords, right);
		float z_y = diffZ(TexCoords, down);

		float mean_curv = divN(TexCoords);

		depthSample += mean_curv * dt;

		FragColor = vec3(depthSample, 0.0f, 0.0f);
	}
}