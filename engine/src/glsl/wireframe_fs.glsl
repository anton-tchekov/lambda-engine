#version 330 core

layout (location = 0) out vec4 fsColor;

uniform vec4 uColor;

void main()
{
	fsColor = uColor;
}

