#version 330 core

struct PointLight
{
	vec3 position;
	vec3 color;
};

in vec2 TexCoords;

out vec4 FragColor;

const float PI = 3.14159265359;

uniform samplerCube irradianceMap;
uniform samplerCube preFilterEnvMap;
uniform sampler2D brdfLUT;

uniform vec3 viewPos;
uniform mat4 proj;

uniform vec3 uAlbedo;
uniform float uRoughness;
uniform float uMetallic;
uniform sampler2D particleDepth;
uniform sampler2D particleThickness;

uniform int nPointLights;
uniform PointLight pointLights[5];

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

void main()
{
	float depth = LinearizeDepth(texture(particleDepth, TexCoords).r) / 100.0f;

	if (depth > 0.5f)
	{
		discard;
	}

	vec3 albedo = glm::vec3(1.0f, 1.0f, 1.0f);
	float roughness = 0.04f;
	float metallic = 0.0f;

	vec2 texelSize = 1.0f / textureSize(particleDepth, 0);
	vec3 Position = getPosition(TexCoords);

	vec3 ddx = getPosition(TexCoords + vec2(texelSize.x, 0.0f)) - Position;
	vec3 ddx2 = Position - getPosition(TexCoords - vec2(texelSize.x, 0.0f));

	if (abs(ddx.z) > abs(ddx2.z))
	{
		ddx = ddx2;
	}

	vec3 ddy = getPosition(TexCoords + vec2(0.0f, texelSize.y)) - Position;
	vec3 ddy2 = Position - getPosition(TexCoords - vec2(0.0f, texelSize.y));

	if (abs(ddy.z) > abs(ddy2.z))
	{
		ddy = ddy2;
	}

	vec3 N = cross(ddx, ddy);

	N = normalize(N);

	
	vec3 V = normalize(viewPos - Position);
	vec3 R = reflect(-V, N); 

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);

	vec3 Lo = vec3(0.0f);
	for (int i = 0; i < nPointLights; i++)
	{
		vec3 L = normalize(pointLights[i].position - Position);
		vec3 H = normalize(V + L);
		float distance = length(pointLights[i].position - Position);
		float attenuation = 1.0f / (distance * distance);
		vec3 radiance = pointLights[i].color * attenuation;

		float NDF = DistributionGGX(N, H, roughness);
		float G = GeometrySmith(N, V, L, roughness);
		vec3 F = fresnelSchlick(max(dot(H, V), 0.0f), F0);

		vec3 nominator = NDF * G * F;
		float denominator = 4 * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f) + 0.001f;
		vec3 brdf = nominator / denominator;

		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;
		kD *= 1.0f - metallic;

		float NdotL = max(dot(N, L), 0.0f);

		Lo += (kD * albedo / PI + brdf) * radiance * NdotL;
	}

	vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);

	vec3 kS = F;
	vec3 kD = 1.0 - kS;
	kD *= 1.0 - metallic;

	
	vec3 irradiance = texture(irradianceMap, N).rgb;
	float thickness = texture(particleThickness, TexCoords).r;
	vec3 diffuse = irradiance * albedo;
	diffuse.r *= exp(-0.02f*thickness);
	diffuse.g *= exp(-0.02f*thickness);
	diffuse.b *= exp(-0.005f*thickness);

	const float MAX_REFLECTION_LOD = 4.0f;
	vec3 prefilteredColor = textureLod(preFilterEnvMap, R,  roughness * MAX_REFLECTION_LOD).rgb;  
	vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
	vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

	vec3 ambient = kD * diffuse + specular;
	
	vec3 color = ambient + Lo;

	// HDR / Gamma correction
	color = color / (color + vec3(1.0f));
	color = pow(color, vec3(1.0/2.2));

	thickness = thickness / 5.0f;
	FragColor = vec4(diffuse, thickness);
	gl_FragDepth = texture(particleDepth, TexCoords).r;

}