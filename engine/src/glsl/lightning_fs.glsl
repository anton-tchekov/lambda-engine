#version 330 core

layout (location = 0) out vec4 fsColor;

in vec4 gsColor;
uniform float uBrightness;

void main()
{
	fsColor = uBrightness * gsColor;
}

