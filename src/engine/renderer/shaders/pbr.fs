#version 330 core

struct PointLight
{
	vec3 position;
	vec3 color;
};

in vec3 Position;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

const float PI = 3.14159265359;

uniform vec3 viewPos;

uniform vec3 uAlbedo;
uniform sampler2D albedoMap;
uniform float uRoughness;
uniform sampler2D roughnessMap;
uniform float uMetallic;
uniform sampler2D metallicMap;

uniform int nPointLights;
uniform PointLight pointLights[5];

vec3 fresnelSchlick(float cosTheta, vec3 F0, float roughness) 
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
	vec3 albedo = uAlbedo + texture(albedoMap, TexCoords).rgb;
	float roughness = uRoughness + texture(roughnessMap, TexCoords).r;
	float metallic = uMetallic + texture(metallicMap, TexCoords).r;

	vec3 N = normalize(Normal);
	vec3 V = normalize(viewPos - Position);

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
		vec3 F = fresnelSchlick(max(dot(N, V), 0.0f), F0, roughness);

		vec3 nominator = NDF * G * F;
		float denominator = 4 * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f) + 0.001f;
		vec3 brdf = nominator / denominator;

		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;
		kD *= 1.0f - metallic;

		float NdotL = max(dot(N, L), 0.0f);

		Lo += (kD * albedo / PI + brdf) * radiance * NdotL;
	}

	vec3 ambient = vec3(0.03f) * albedo;
	
	vec3 color = ambient + Lo;

	// HDR / Gamma correction
	color = color / (color + vec3(1.0f));
	color = pow(color, vec3(1.0/2.2));

	FragColor = vec4(color, 1.0f);

}