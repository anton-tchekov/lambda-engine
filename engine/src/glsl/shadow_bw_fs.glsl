#version 330 core

layout (location = 0) out vec4 fsColor;

in vec2 vsTC;
in vec3 vsWorld, vsNormal;
in vec4 vsLighting;

uniform vec3 uDL_Position;
uniform sampler2D uSamplerShadow;

float ShadowMap(vec4 lspos)
{
	vec3 proj_coords = (lspos.xyz / lspos.w) * 0.5 + 0.5;
	float closest_depth = texture(uSamplerShadow, proj_coords.xy).r;
	float current_depth = proj_coords.z;
	vec3 normal = normalize(vsNormal);
	vec3 light_direction = normalize(uDL_Position - vsWorld);
	float bias = max(0.05 * (1.0 - dot(normal, light_direction)), 0.005);
	float shadow = 0.0;
	vec2 texel_size = 1.0 / textureSize(uSamplerShadow, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcf_depth = texture(uSamplerShadow, proj_coords.xy + vec2(x, y) * texel_size).r;
			shadow += int(current_depth - bias > pcf_depth);
		}
	}

	/*float pcf_depth = texture(uSamplerShadow, proj_coords.xy).r;
	shadow += int(current_depth - bias > pcf_depth);*/

	/*shadow /= 9.0;*/
	if(proj_coords.z > 1.0)
	{
		shadow = 0.0;
	}

	return shadow;
}

void main()
{
	float result = max((1.0 - ShadowMap(vsLighting)), 0.0);
	fsColor = vec4(result, result, result, 1.0);
}

