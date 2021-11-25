#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in mat4 inMatrix;

uniform mat4 uPerspective;

void main()
{
	vec3 vsPosition = vec3(inMatrix * vec4(inPosition, 1.0));
	gl_Position = uPerspective * vec4(vsPosition, 1.0);
}

