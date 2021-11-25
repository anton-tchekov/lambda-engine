#version 330 core

layout (location = 0) in vec3 inPosition;

out vec3 vsCubemapCoordinates;

uniform mat4 uPerspective;

void main()
{
	vsCubemapCoordinates = inPosition;
	gl_Position = (uPerspective * vec4(inPosition, 1.0)).xyww;
}

