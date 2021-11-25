#version 330 core
layout (lines) in;
layout (triangle_strip, max_vertices = 16) out;

in vec3 vsExtend[];
out vec4 gsColor;

uniform bool uFront;
uniform mat4 uPerspective;
uniform float uThickness;

void main()
{
	float glow = 0.9;
	float part = 0.2;
	float ap = 1.0 - part;
	vec3 blue = vec3(35.0 / 255.0, 180.0 / 255.0, 234.0 / 255.0);
	vec3 inner = vec3(234.0 / 255.0, 252.0 / 255.0, 253.0 / 255.0);
	vec3 blu2 = vec3(185.0 / 255.0, 245.0 / 255.0, 255.0 / 255.0);

	vec3 t[2];
	t[0] = vsExtend[0] * uThickness;
	t[1] = vsExtend[1] * uThickness;

	if(uFront)
	{
		gl_Position = uPerspective * vec4(gl_in[0].gl_Position.xyz - t[0].xyz * part, 1.0);
		gsColor = vec4(blu2, 1.0f);
		EmitVertex();
		gl_Position = uPerspective * vec4(gl_in[0].gl_Position.xyz, 1.0);
		gsColor = vec4(inner, 1.0f);
		EmitVertex();
		gl_Position = uPerspective * vec4(gl_in[1].gl_Position.xyz - t[1].xyz * part, 1.0);
		gsColor = vec4(blu2, 1.0f);
		EmitVertex();
		gl_Position = uPerspective * vec4(gl_in[1].gl_Position.xyz, 1.0);
		gsColor = vec4(inner, 1.0f);
		EmitVertex();
		EndPrimitive();

		gl_Position = uPerspective * vec4(gl_in[0].gl_Position.xyz, 1.0);
		gsColor = vec4(inner, 1.0f);
		EmitVertex();
		gl_Position = uPerspective * vec4(gl_in[0].gl_Position.xyz + t[0].xyz * part, 1.0);
		gsColor = vec4(blu2, 1.0f);
		EmitVertex();
		gl_Position = uPerspective * vec4(gl_in[1].gl_Position.xyz, 1.0);
		gsColor = vec4(inner, 1.0f);
		EmitVertex();
		gl_Position = uPerspective * vec4(gl_in[1].gl_Position.xyz + t[1].xyz * part, 1.0);
		gsColor = vec4(blu2, 1.0f);
		EmitVertex();
		EndPrimitive();

		gl_Position = uPerspective * vec4(gl_in[0].gl_Position.xyz - t[0].xyz * glow, 1.0);
		gsColor = vec4(blue, 0.0f);
		EmitVertex();
		gl_Position = uPerspective * vec4(gl_in[0].gl_Position.xyz - t[0].xyz * part, 1.0);
		gsColor = vec4(blue, 0.0f);
		EmitVertex();
		gl_Position = uPerspective * vec4(gl_in[1].gl_Position.xyz - t[1].xyz * glow, 1.0);
		gsColor = vec4(blue, 0.0f);
		EmitVertex();
		gl_Position = uPerspective * vec4(gl_in[1].gl_Position.xyz - t[1].xyz * part, 1.0);
		gsColor = vec4(blue, 0.0f);
		EmitVertex();
		EndPrimitive();

		gl_Position = uPerspective * vec4(gl_in[0].gl_Position.xyz + t[0].xyz * part, 1.0);
		gsColor = vec4(blue, 0.0f);
		EmitVertex();
		gl_Position = uPerspective * vec4(gl_in[0].gl_Position.xyz + t[0].xyz * glow, 1.0);
		gsColor = vec4(blue, 0.0f);
		EmitVertex();
		gl_Position = uPerspective * vec4(gl_in[1].gl_Position.xyz + t[1].xyz * part, 1.0);
		gsColor = vec4(blue, 0.0f);
		EmitVertex();
		gl_Position = uPerspective * vec4(gl_in[1].gl_Position.xyz + t[1].xyz * glow, 1.0);
		gsColor = vec4(blue, 0.0f);
		EmitVertex();
		EndPrimitive();
	}
	else
	{
		gl_Position = uPerspective * vec4(gl_in[0].gl_Position.xyz - t[0].xyz * glow, 1.0);
		gsColor = vec4(blue, 0.0f);
		EmitVertex();
		gl_Position = uPerspective * vec4(gl_in[0].gl_Position.xyz - t[0].xyz * part, 1.0);
		gsColor = vec4(blue, 1.0f);
		EmitVertex();
		gl_Position = uPerspective * vec4(gl_in[1].gl_Position.xyz - t[1].xyz * glow, 1.0);
		gsColor = vec4(blue, 0.0f);
		EmitVertex();
		gl_Position = uPerspective * vec4(gl_in[1].gl_Position.xyz - t[1].xyz * part, 1.0);
		gsColor = vec4(blue, 1.0f);
		EmitVertex();
		EndPrimitive();

		gl_Position = uPerspective * vec4(gl_in[0].gl_Position.xyz + t[0].xyz * part, 1.0);
		gsColor = vec4(blue, 1.0f);
		EmitVertex();
		gl_Position = uPerspective * vec4(gl_in[0].gl_Position.xyz + t[0].xyz * glow, 1.0);
		gsColor = vec4(blue, 0.0f);
		EmitVertex();
		gl_Position = uPerspective * vec4(gl_in[1].gl_Position.xyz + t[1].xyz * part, 1.0);
		gsColor = vec4(blue, 1.0f);
		EmitVertex();
		gl_Position = uPerspective * vec4(gl_in[1].gl_Position.xyz + t[1].xyz * glow, 1.0);
		gsColor = vec4(blue, 0.0f);
		EmitVertex();
		EndPrimitive();
	}
}
