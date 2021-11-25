#version 330 core

layout (location = 0) in vec3 inPosition;

uniform mat4 uPerspective;

void main()
{
	gl_Position = uPerspective * vec4(inPosition, 1.0);
}

