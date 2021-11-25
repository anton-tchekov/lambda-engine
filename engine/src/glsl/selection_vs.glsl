#version 330 core

layout (location = 0) in vec3 inPosition;

flat out float vsSelectionID;

uniform mat4 uPerspective, uPositionMatrix[32];
uniform float uSelectionID[32];

void main()
{
	vsSelectionID = uSelectionID[gl_InstanceID];
	gl_Position = uPerspective * uPositionMatrix[gl_InstanceID] * vec4(inPosition, 1.0);
}

