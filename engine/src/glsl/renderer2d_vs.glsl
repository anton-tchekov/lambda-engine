#version 330 core

layout (location = 0) in vec3 inVertexSrc;
layout (location = 1) in vec2 inVertexDst;
layout (location = 2) in vec4 inColor;

out vec4 vsColor;
out vec3 vsUV;

uniform float uTextureSize;
uniform mat4 uProjection;

void main()
{
	vsColor = inColor;
	vsUV = vec3(inVertexSrc.xy / uTextureSize, inVertexSrc.z);
	gl_Position = vec4((uProjection * vec4(inVertexDst.xy, 0.0, 1.0)).xy, 0.0, 1.0);
}

