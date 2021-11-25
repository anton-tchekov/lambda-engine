#version 330 core

layout (location = 0) in vec3 inPosition;

uniform mat4 uPerspective, uPositionMatrix[32];

void main()
{
	mat4 m = uPositionMatrix[gl_InstanceID];
	vec3 PW = vec3(m * vec4(inPosition, 1.0));
	gl_Position = uPerspective * vec4(PW, 1.0);
}

