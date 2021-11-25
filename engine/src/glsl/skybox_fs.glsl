#version 330 core

layout (location = 0) out vec4 fsColor;
layout (location = 1) out vec4 fsGlow;

in vec3 vsCubemapCoordinates;

uniform samplerCube uCubemap;

void main()
{
	fsColor = texture(uCubemap, vsCubemapCoordinates);
	fsGlow = vec4(0.0, 0.0, 0.0, 1.0);
}

