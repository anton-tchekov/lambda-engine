#version 330 core

layout (location = 0) out vec4 fsColor;

flat in float vsSelectionID;

void main()
{
	fsColor = vec4(vsSelectionID, 0.0, 0.0, 1.0);
}

