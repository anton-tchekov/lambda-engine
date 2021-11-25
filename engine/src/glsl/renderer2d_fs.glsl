#version 330 core

in vec4 vsColor;
in vec3 vsUV;

out vec4 fsColor;

uniform sampler2DArray uTexture;

void main()
{
	fsColor = texture(uTexture, vsUV).rgba * vsColor;
}

