#version 330 core



in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D depthImage;
uniform bool horizontal;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; 
    return (2.0 * 0.1f * 100.0f) / (100.0f + 0.1f - z * (100.0f - 0.1f));
}

void main()
{
	vec2 texelSize = 1.0f / textureSize(depthImage, 0);
	float depth = texture(depthImage, TexCoords).r;

	float ddx = texture(depthImage, TexCoords + vec2(texelSize.x, 0.0f)).r - depth;
	float ddxc = 1.0f - abs(depth - ddx);

	float ddx2 = texture(depthImage, TexCoords - vec2(texelSize.x, 0.0f)).r - depth;
	float ddx2c = 1.0f - abs(depth - ddx2);

	float ddy = texture(depthImage, TexCoords + vec2(0.0f, texelSize.y)).r - depth;
	float ddyc = 1.0f - abs(depth - ddy);

	float ddy2 = texture(depthImage, TexCoords - vec2(0.0f, texelSize.y)).r - depth;
	float ddy2c = 1.0f - abs(depth - ddy2);

	gl_FragDepth = depth + (1.0f/4.0f)*((ddx)+(ddx2)+(ddy)+(ddy2));

	/*float weight[8] = float[] (0.197448, 0.174697, 0.120999, 0.065602, 0.02784, 0.009246 , 0.002403, 0.000489);
	vec2 tex_offset = 1.0 / textureSize(depthImage, 0);
	float depth = texture(depthImage, TexCoords).r;
	float result = depth * weight[0];
	float normalization = weight[0];

	if(horizontal)
    {
        for(int i = 1; i <8; ++i)
        {
			float sample = texture(depthImage, TexCoords + vec2(tex_offset.x * i, 0.0)).r;
			float closeness = 1.0f - abs(sample - depth);
			float sampleWeight = closeness * weight[i];
            result += sample * sampleWeight;
			normalization += sampleWeight;

			sample = texture(depthImage, TexCoords - vec2(tex_offset.x * i, 0.0)).r;
			closeness = 1.0f - abs(sample - depth);
			sampleWeight = closeness * weight[i];
            result += sample * sampleWeight;
			normalization += sampleWeight;
        }
    }
    else
    {
        for(int i = 1; i < 8; ++i)
        {
			float sample = texture(depthImage, TexCoords + vec2(0.0, tex_offset.y * i)).r;
			float closeness = 1.0f - abs(sample - depth);
			float sampleWeight = closeness * weight[i];
            result += sample * sampleWeight;
			normalization += sampleWeight;

			sample = texture(depthImage, TexCoords - vec2(0.0, tex_offset.y * i)).r;
			closeness = 1.0f - abs(sample - depth);
			sampleWeight = closeness * weight[i];
            result += sample * sampleWeight;
			normalization += sampleWeight;
        }
    }

	gl_FragDepth =  result / normalization;*/

}