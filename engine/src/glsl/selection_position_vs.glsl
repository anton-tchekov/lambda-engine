#version 330 core

layout (location = 0) in vec3 inPosition;

out vec4 vsPosition;

uniform mat4 uPerspective, uPositionMatrix[32];

void main()
{
	vsPosition = uPositionMatrix[gl_InstanceID] * vec4(inPosition, 1.0);
	gl_Position = uPerspective * vsPosition;
}

