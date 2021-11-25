#version 330 core

#define NUM_POINT_LIGHTS 4

layout (location = 0) out vec4 fsColor;

in vec3 vsPosition, vsNormal;
in vec2 vsTC_Diffuse, vsTC_Emissive;

uniform vec3 uPL_Position[NUM_POINT_LIGHTS],
		uPL_Color[NUM_POINT_LIGHTS];

uniform float uPL_Constant[NUM_POINT_LIGHTS],
		uPL_Linear[NUM_POINT_LIGHTS],
		uPL_Quadratic[NUM_POINT_LIGHTS],
		uShadow, uDark;

uniform sampler2D uTexture;

void main()
{
	float l_diffuse, attenuation, distance;
	vec3 normal, light_direction;
	vec4 diffuse, emissive, result;
	normal = normalize(vsNormal);
	diffuse = texture(uTexture, vsTC_Diffuse).rgba;
	emissive = texture(uTexture, vsTC_Emissive).rgba;

	result = emissive;
	result.a += diffuse.a;
	result.rgb += diffuse.rgb * uDark;

	light_direction = normalize(uPL_Position[0] - vsPosition);
	l_diffuse = max(dot(normal, light_direction), 0.0);
	distance = length(uPL_Position[0] - vsPosition);
	attenuation = 1.0 / (uPL_Constant[0] + uPL_Linear[0] * distance + uPL_Quadratic[0] * distance * distance);
	result.rgb += diffuse.rgb * l_diffuse * uPL_Color[0] * attenuation * uShadow;

	light_direction = normalize(uPL_Position[1] - vsPosition);
	l_diffuse = max(dot(normal, light_direction), 0.0);
	distance = length(uPL_Position[1] - vsPosition);
	attenuation = 1.0 / (uPL_Constant[1] + uPL_Linear[1] * distance + uPL_Quadratic[1] * distance * distance);
	result.rgb += diffuse.rgb * l_diffuse * uPL_Color[1] * attenuation;

	light_direction = normalize(uPL_Position[2] - vsPosition);
	l_diffuse = max(dot(normal, light_direction), 0.0);
	distance = length(uPL_Position[2] - vsPosition);
	attenuation = 1.0 / (uPL_Constant[2] + uPL_Linear[2] * distance + uPL_Quadratic[2] * distance * distance);
	result.rgb += diffuse.rgb * l_diffuse * uPL_Color[2] * attenuation;

	light_direction = normalize(uPL_Position[3] - vsPosition);
	l_diffuse = max(dot(normal, light_direction), 0.0);
	distance = length(uPL_Position[3] - vsPosition);
	attenuation = 1.0 / (uPL_Constant[3] + uPL_Linear[3] * distance + uPL_Quadratic[3] * distance * distance);
	result.rgb += diffuse.rgb * l_diffuse * uPL_Color[3] * attenuation;

	fsColor = result;
}

