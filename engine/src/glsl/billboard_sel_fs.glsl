#version 330 core

layout (location = 0) out vec4 fsColor;

in vec3 vsPosition, vsSelection;
in vec2 vsTC_Diffuse, vsTC_Emissive;

uniform sampler2D uTexture;

void main()
{
	if(texture(uTexture, vsTC_Diffuse).a < 0.2 && texture(uTexture, vsTC_Emissive).a < 0.2)
	{
		discard;
	}

	fsColor = vec4(vsSelection, 1.0);
}

