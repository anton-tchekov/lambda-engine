#version 330 core

#define NUM_POINT_LIGHTS 4

layout (location = 0) out vec4 fsColor;

in vec3 vsPosition, vsViewPosition, vsNormal, vsWorld, vsDL_Position;
in vec2 vsTC;

in vec3 vsPL_Position[NUM_POINT_LIGHTS];

uniform sampler2DArray uSamplerDiffuse, uSamplerSpecular, uSamplerEmissive, uSamplerNormal;
uniform sampler2D uSamplerShadow;

uniform bool uDL_Used;
uniform vec3 uDL_Color;

uniform vec3 uPL_Color[NUM_POINT_LIGHTS];
uniform float uPL_Constant[NUM_POINT_LIGHTS],
		uPL_Linear[NUM_POINT_LIGHTS],
		uPL_Quadratic[NUM_POINT_LIGHTS],
		uDark, uShadow;

uniform vec2 uResolution;
uniform float uShininess;
uniform vec3 uColorDiffuse, uColorSpecular, uColorEmissive;
uniform float uLayerDiffuse, uLayerSpecular, uLayerEmissive, uLayerNormal;

float ShadowMap(vec2 pos)
{
	return texture(uSamplerShadow, pos).r;
}

void main()
{
	float l_diffuse, l_specular, attenuation, l_distance;
	vec4 diffuse;
	vec3 normal, light_direction, view_direction, result, specular, emissive, halfway;
	diffuse = texture(uSamplerDiffuse, vec3(vsTC, uLayerDiffuse)) * vec4(uColorDiffuse, 1.0);
	if(diffuse.a < 0.5)
	{
		discard;
	}

	view_direction = normalize(vsViewPosition - vsPosition);
	specular = texture(uSamplerSpecular, vec3(vsTC, uLayerSpecular)).rgb * uColorSpecular;
	emissive = texture(uSamplerEmissive, vec3(vsTC, uLayerEmissive)).rgb * uColorEmissive;
	normal = texture(uSamplerNormal, vec3(vsTC, uLayerNormal)).rgb * 2.0 - 1.0;
	result = diffuse.rgb * uDark + emissive;

	if(uDL_Used)
	{
		light_direction = normalize(vsDL_Position - vsPosition);
		l_diffuse = max(dot(normal, light_direction), 0.0);
		halfway = normalize(light_direction + view_direction);
		l_specular = pow(max(dot(normal, halfway), 0.0), uShininess);
		result += max((diffuse.rgb * l_diffuse + specular * l_specular) * uDL_Color * ShadowMap(gl_FragCoord.xy / uResolution), 0.0);
	}

	light_direction = normalize(vsPL_Position[0] - vsPosition);
	l_diffuse = max(dot(normal, light_direction), 0.0);
	l_distance = length(vsPL_Position[0] - vsPosition);
	attenuation = 1.0 / (uPL_Constant[0] + uPL_Linear[0] * l_distance + uPL_Quadratic[0] * l_distance * l_distance);
	halfway = normalize(light_direction + view_direction);
	l_specular = pow(max(dot(normal, halfway), 0.0), uShininess);
	result += max((diffuse.rgb * l_diffuse + specular * l_specular) * uPL_Color[0] * attenuation * uShadow, 0.0);

	light_direction = normalize(vsPL_Position[1] - vsPosition);
	l_diffuse = max(dot(normal, light_direction), 0.0);
	l_distance = length(vsPL_Position[1] - vsPosition);
	attenuation = 1.0 / (uPL_Constant[1] + uPL_Linear[1] * l_distance + uPL_Quadratic[1] * l_distance * l_distance);
	halfway = normalize(light_direction + view_direction);
	l_specular = pow(max(dot(normal, halfway), 0.0), uShininess);
	result += max((diffuse.rgb * l_diffuse + specular * l_specular) * uPL_Color[1] * attenuation, 0.0);

	light_direction = normalize(vsPL_Position[2] - vsPosition);
	l_diffuse = max(dot(normal, light_direction), 0.0);
	l_distance = length(vsPL_Position[2] - vsPosition);
	attenuation = 1.0 / (uPL_Constant[2] + uPL_Linear[2] * l_distance + uPL_Quadratic[2] * l_distance * l_distance);
	halfway = normalize(light_direction + view_direction);
	l_specular = pow(max(dot(normal, halfway), 0.0), uShininess);
	result +=  max((diffuse.rgb * l_diffuse + specular * l_specular) * uPL_Color[2] * attenuation, 0.0);

	light_direction = normalize(vsPL_Position[3] - vsPosition);
	l_diffuse = max(dot(normal, light_direction), 0.0);
	l_distance = length(vsPL_Position[3] - vsPosition);
	attenuation = 1.0 / (uPL_Constant[3] + uPL_Linear[3] * l_distance + uPL_Quadratic[3] * l_distance * l_distance);
	halfway = normalize(light_direction + view_direction);
	l_specular = pow(max(dot(normal, halfway), 0.0), uShininess);
	result +=  max((diffuse.rgb * l_diffuse + specular * l_specular) * uPL_Color[3] * attenuation, 0.0);

	fsColor = vec4(result, 1.0);
}

