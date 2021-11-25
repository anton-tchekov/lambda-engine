#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec4 inNormal;

out vec4 vsLighting;
out vec3 vsWorld, vsNormal;
out vec2 vsTC;

uniform mat4 uPerspective, uPositionMatrix[32], uLightMatrix;

void main()
{
	mat4 m = uPositionMatrix[gl_InstanceID];
	vec3 PW = vec3(m * vec4(inPosition, 1.0));
	vsWorld = PW;
	vsNormal = inNormal.xyz;
	vsLighting = uLightMatrix * vec4(PW, 1.0);
	gl_Position = uPerspective * vec4(PW, 1.0);
}

