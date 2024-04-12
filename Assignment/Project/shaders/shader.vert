#version 450

layout(binding = 0) uniform UBO
{
	mat4 View;
	mat4 Projection;
} VPMatrix;

layout(push_constant) uniform MODEL
{
	mat4 Model;
} MMatrix;

layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in vec3 vertexColor;

layout(location = 0) out vec3 fragColor;

void main()
{
	gl_Position = VPMatrix.Projection * VPMatrix.View * MMatrix.Model *	vec4(vertexPosition, 0.0, 1.0);
	fragColor = vertexColor;
}