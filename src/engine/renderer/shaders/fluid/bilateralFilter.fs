#version 330 core

const float PI = 3.14159265359;

in vec2 TexCoords;

out vec3 FragColor;

uniform sampler2D image;
uniform bool horizontal;

float Gaussian(float sigma, float x)
{
	return (1.0f / (2.0f*PI*sigma*sigma))*exp(-(x*x)/(2.0f*sigma*sigma));
}

void main()
{
	vec2 tex_offset = 1.0 / textureSize(image, 0);
	float sigma_r = 10.0f;
	float value = texture(image, TexCoords).r;
	float result = 0;
	float normalization = 0;

	if(horizontal)
    {
        for(int i = 1; i <5; ++i)
        {
			float sample = texture(image, TexCoords + vec2(tex_offset.x * i, 0.0)).r;
			float distance = tex_offset.x*i;
			float s = Gaussian(4.0f, distance);
			float r = Gaussian(sigma_r, abs(sample-value));
			
			result += sample * s * r;
			normalization += s * r;

			sample = texture(image, TexCoords - vec2(tex_offset.x * i, 0.0)).r;
			r = Gaussian(sigma_r, abs(sample-value));
			result += sample * s * r;
			normalization += s * r;
        }
    }
    else
    {
        for(int i = 1; i < 5; ++i)
        {
			float sample = texture(image, TexCoords + vec2(0.0, tex_offset.y * i)).r;
			float distance = tex_offset.y * i;
			float s = Gaussian(4.0f, distance);
			float r = Gaussian(sigma_r, abs(sample-value));

			result += sample * s * r;
			normalization += s * r;

			sample = texture(image, TexCoords - vec2(0.0, tex_offset.y * i)).r;
			r = Gaussian(sigma_r, abs(sample-value));
			result += sample * s * r;
			normalization += s * r;
        }
    }

	if (normalization > 0.0f)
	{
		result /= normalization;
	}

	FragColor = vec3(result, 0.0f, 0.0f);

	

}