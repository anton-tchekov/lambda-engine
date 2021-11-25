#version 330 core

#define TEXTURE_SIZE (1.0 / 2048.0)

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec4 inNormal;
layout (location = 2) in vec2 inTC_Diffuse;
layout (location = 3) in vec2 inTC_Emissive;
layout (location = 4) in vec4 inSelection;

out vec3 vsPosition, vsSelection;
out vec2 vsTC_Diffuse, vsTC_Emissive;

uniform mat4 uPerspective;

void main()
{
	vsPosition = inPosition;
	vsSelection = inSelection.xyz;
	vsTC_Diffuse = inTC_Diffuse * TEXTURE_SIZE;
	vsTC_Emissive = inTC_Emissive * TEXTURE_SIZE;
	gl_Position = uPerspective * vec4(vsPosition, 1.0);
}
