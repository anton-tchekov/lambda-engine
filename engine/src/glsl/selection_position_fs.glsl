#version 330 core

layout (location = 0) out vec4 fsColor;

in vec4 vsPosition;

void main()
{
	fsColor = vec4(vsPosition.rgb, 1.0);
}

