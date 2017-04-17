#version 330 core

struct PointLight
{
	vec3 position;
	vec3 color;
};

in vec2 TexCoords;

out vec4 FragColor;

const float PI = 3.14159265359;
const float width = 1280.0f;
const float height = 720.0f;

uniform samplerCube irradianceMap;
uniform samplerCube preFilterEnvMap;
uniform sampler2D brdfLUT;

uniform vec3 viewPos;
uniform mat4 proj;
uniform mat4 view;

uniform vec3 uAlbedo;
uniform float uRoughness;
uniform float uMetallic;
uniform sampler2D sceneColor;
uniform sampler2D sceneDepth;
uniform sampler2D particleDepth;
uniform sampler2D particleThickness;
uniform sampler2D noise;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; 
    return (2.0 * 0.1f * 100.0f) / (100.0f + 0.1f - z * (100.0f - 0.1f));
}

vec3 getPosition(vec2 texCoords)
{
	float z = texture(particleDepth, texCoords).r * 2.0f - 1.0f;
	float x = texCoords.x * 2.0f - 1.0f;
	float y = texCoords.y * 2.0f - 1.0f;
	vec4 position = vec4(x, y, z, 1.0f);

	position = inverse(proj) * position;

	return position.xyz / position.w;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}   

float DistributionGGX(vec3 N, vec3 H, float roughness) 
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0f);
    float NdotH2 = NdotH*NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0f);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) 
{
    float r = roughness + 1.0f;
    float k = (r*r) / 8.0f;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) 
{
    float NdotV = max(dot(N, V), 0.0f);
    float NdotL = max(dot(N, L), 0.0f);
    float ggx1 = GeometrySchlickGGX(NdotV, roughness);
    float ggx2 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

float projectZ(float z)
{
	float A = proj[2].z;
	float B = proj[3].z;

	return 0.5f*(-A*z + B) / z + 0.5f;
}

float diffZ(vec2 uv, vec2 offset)
{
	float dp = texture(particleDepth, uv+offset).x;
	float dm = texture(particleDepth, uv-offset).x;

	if (dm == 0.0f || dp == 0.0f) return 0.0f;

	return (dp - dm) / 2.0f;
}

vec3 computeNormal(vec2 uv, float w, float h)
{
	vec2 texelSize = 1.0f / textureSize(particleDepth, 0);
	vec2 right = vec2(texelSize.x, 0.0f);
	vec2 down = vec2(0.0f, texelSize.y);

	float z = texture(particleDepth, TexCoords).x;
	float z_x = diffZ(uv, right);
	float z_y = diffZ(uv, down);

	float Cx = -2.0f / (width*w);
	float Cy = -2.0f / (height*h);

	float sx = floor(uv.x*(width-1.0f));
	float sy = floor(uv.y*(height-1.0f));
	float Wx = (width-2*sx)/(width*w);
	float Wy = (height-2*sy)/(height*h);

	vec3 _dx = vec3(Cx*z+Wx*z_x, Wy*z_x, z_x);
	vec3 _dy = vec3(Wx*z_y, Cy*z+Wy*z_y, z_y);

	vec3 N = cross(_dx, _dy);

	return normalize(inverse(mat3(view))*N);
}

vec3 uvToWorld(vec2 uv, float depth, float w, float h)
{
	float z = depth * 2.0f - 1.0f;
	float x = uv.x * 2.0f - 1.0f;
	float y = uv.y * 2.0f - 1.0f;

	vec4 clipSpacePosition = vec4(x,y,z,1.0f);
	vec4 viewSpacePosition = inverse(proj) * clipSpacePosition;

	viewSpacePosition /= viewSpacePosition.w;

	return (inverse(view) * viewSpacePosition).xyz;
}

void main()
{
	float depth = texture(particleDepth, TexCoords).r;
	float bufferDepth = projectZ(depth);

	if (depth < 1.0f)
	{
		FragColor = vec4(texture(sceneColor, TexCoords).rgb, 1.0f);
	} 
	else if (bufferDepth == 0.0f)
	{
		FragColor = vec4(texture(sceneColor, TexCoords).rgb, 1.0f);
	}
	else if (bufferDepth > texture(sceneDepth, TexCoords).r)
	{
		FragColor = vec4(texture(sceneColor, TexCoords).rgb, 1.0f);
	}
	else
	{
		float w = proj[0][0];
		float h = proj[1][1];

		vec3 N = normalize(computeNormal(TexCoords, w, h));
		vec3 Position = uvToWorld(TexCoords, bufferDepth, w, h);
		vec3 V = normalize(viewPos - Position);
		vec3 R = reflect(-V, N); 

		float thickness = texture(particleThickness, TexCoords).x;
		float transparancy = 1.0f - exp(-0.5*thickness);
		vec3 albedo =   mix(texture(sceneColor, TexCoords + N.xy * thickness).rgb, vec3(0.0f, 0.7f, 0.7f), transparancy);
		float roughness = 0.1f;
		float metallic = 0.0f;
		
		vec3 F0 = vec3(0.04);
		F0 = mix(F0, albedo, metallic);

		vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);

		vec3 kS = F;
		vec3 kD = 1.0 - kS;
		kD *= 1.0 - metallic;

		vec3 irradiance = texture(irradianceMap, N).rgb;
		vec3 diffuse = albedo;

		diffuse.r *= exp(-0.6*thickness);
		diffuse.g *= exp(-0.2*thickness);
		diffuse.b *= exp(-0.05*thickness);

		const float MAX_REFLECTION_LOD = 4.0f;
		vec3 prefilteredColor = textureLod(preFilterEnvMap, R,  roughness * MAX_REFLECTION_LOD).rgb;  
		vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
		vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

		vec3 ambient = kD * diffuse + specular;

		vec3 color = ambient;

		// HDR / Gamma correction
		color = color / (color + vec3(1.0f));
		color = pow(color, vec3(1.0/2.2));

		FragColor = vec4(color, 1.0f);
	}
		
	
	
	
	
	
	
	/*float sceneDepthValue = texture(sceneDepth, TexCoords).r;

	if (depth > 50.0f || depth == 0.0f)
	{
		discard;
	}

	vec4 depthVec = proj * vec4(0.0f, 0.0f, depth, 1.0f);
	depthVec.xyz = depthVec.xyz / depthVec.w;
	depthVec.xyz = depthVec.xyz*0.5f + 0.5f;

	if (sceneDepthValue <= depthVec.z)
	{
		FragColor = vec4(texture(sceneColor, TexCoords).rgb, 1.0f);
	}
	else
	{
		vec3 albedo = vec3(1.0f, 1.0f, 1.0f);
		float roughness = 0.3f;
		float metallic = 0.0f;

		vec2 texelSize = 1.0f / textureSize(particleDepth, 0);
		vec2 right = vec2(texelSize.x, 0.0f);
		vec2 down = vec2(0.0f, texelSize.y);

		float w = proj[0][0];
		float h = proj[1][1];
		float width = 1280.0f;
		float height = 720.0f;
		float z = depthVec.z;
		float z_x = diffZ(TexCoords, right);
		float z_y = diffZ(TexCoords, down);

	
		
		Position = vec3(inverse(view)* vec4(N, 1.0f));
		N = inverse(mat3(view))*N;
		//N += texture(noise, TexCoords).rgb;

		N = normalize(N);

		vec3 V = normalize(viewPos - Position);
		vec3 R = reflect(-V, N); 

		vec3 F0 = vec3(0.04);
		F0 = mix(F0, albedo, metallic);

		vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);

		vec3 kS = F;
		vec3 kD = 1.0 - kS;
		kD *= 1.0 - metallic;

		float thickness = texture(particleThickness, TexCoords).r*2.0f;
		albedo = texture(sceneColor, TexCoords + N.xy*thickness).rgb;

		vec3 irradiance = texture(irradianceMap, N).rgb;
		vec3 diffuse = irradiance * albedo;

		const float MAX_REFLECTION_LOD = 4.0f;
		vec3 prefilteredColor = textureLod(preFilterEnvMap, R,  roughness * MAX_REFLECTION_LOD).rgb;  
		vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
		vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

		vec3 ambient = kD * diffuse + specular;
	
		vec3 color = ambient;
		color.r *= exp(-0.6f*thickness);
		color.g *= exp(-0.2f*thickness);
		color.b *= exp(-0.05f*thickness);

		// HDR / Gamma correction
		color = color / (color + vec3(1.0f));
		color = pow(color, vec3(1.0/2.2));

		thickness = 1.0f - exp(-3.0f*thickness);

		if (thickness < 0.8f)
		{
			FragColor = vec4(texture(sceneColor, TexCoords).rgb, 1.0f);
		}
		else
		{
			FragColor = vec4(vec3(depth / 100.0f), 1.0f);
		}

		gl_FragDepth = depthVec.z;
	}*/
}