#version 450

layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in vec3 vertexColor;

layout(push_constant) uniform constants
{
	mat4 model;
}objectData;

layout(location = 0) out vec3 fragColor;

void main()
{
	gl_Position = objectData.model * vec4(vertexPosition, 0.0, 1.0);
	fragColor = vertexColor;
}