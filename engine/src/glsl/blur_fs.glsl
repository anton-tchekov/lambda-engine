#version 330 core

layout (location = 0) out vec4 fsColor;

#define RADIUS 5

in vec2 vsTC;

uniform sampler2D uSampler;
uniform bool uHorizontal;

float weight[RADIUS] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
	vec2 tex_offset = 1.0 / textureSize(uSampler, 0);
	vec3 result = texture(uSampler, vsTC).rgb * weight[0];
	if(uHorizontal)
	{
		for(int i = 1; i < RADIUS; ++i)
		{
			result += texture(uSampler, vsTC + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
			result += texture(uSampler, vsTC - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
		}
	}
	else
	{
		for(int i = 1; i < RADIUS; ++i)
		{
			result += texture(uSampler, vsTC + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
			result += texture(uSampler, vsTC - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
		}
	}

	fsColor = vec4(result, 1.0);
}
