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
layout(location = 2) in vec2 vertexTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main()
{
	gl_Position = vec4(vertexPosition, 0.9, 1.0);
	fragColor = vertexColor;
	fragTexCoord = vertexTexCoord;
}