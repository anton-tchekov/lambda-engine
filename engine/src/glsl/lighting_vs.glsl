#version 330 core

#define NUM_POINT_LIGHTS 4

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec4 inNormal;
layout (location = 2) in vec2 inTC;
layout (location = 3) in vec4 inTangent;
layout (location = 4) in vec4 inBitangent;

out vec3 vsPosition, vsViewPosition, vsNormal, vsWorld, vsDL_Position;
out vec2 vsTC;
out vec3 vsPL_Position[NUM_POINT_LIGHTS];

uniform vec3 uViewPosition, uDL_Position;
uniform vec3 uPL_Position[NUM_POINT_LIGHTS];
uniform mat4 uPerspective, uPositionMatrix[32];

void main()
{
	mat4 m = uPositionMatrix[gl_InstanceID];
	mat3 n = mat3(transpose(inverse(m)));
	vec3 N = normalize(n * inNormal.xyz);
	vec3 T = normalize(n * inTangent.xyz);
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);

	vsNormal = N;

	vec3 PW = vec3(m * vec4(inPosition, 1.0));
	vsWorld = PW;

	mat3 TBN = transpose(mat3(T, B, N));
	vsDL_Position = TBN * uDL_Position;
	vsPL_Position[0] = TBN * uPL_Position[0];
	vsPL_Position[1] = TBN * uPL_Position[1];
	vsPL_Position[2] = TBN * uPL_Position[2];
	vsPL_Position[3] = TBN * uPL_Position[3];
	vsViewPosition = TBN * uViewPosition;
	vsPosition = TBN * PW;

	vsTC = inTC;
	gl_Position = uPerspective * vec4(PW, 1.0);
}

