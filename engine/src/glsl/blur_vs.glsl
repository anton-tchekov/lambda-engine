#version 330 core

layout (location = 0) in vec2 inPosition;
layout (location = 1) in vec2 inTC;

out vec2 vsTC;

void main()
{
	vsTC = inTC;
	gl_Position = vec4(inPosition, 0.0, 1.0);
}

