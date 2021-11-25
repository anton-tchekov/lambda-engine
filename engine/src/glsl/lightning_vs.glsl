#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inExtend;

out vec3 vsExtend;

void main()
{
	vsExtend = inExtend;
	gl_Position = vec4(inPosition, 1.0);
}

