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

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec3 vertexNormal;

layout(location = 0) out vec3 fragWorldPos;
layout(location = 1) out vec3 fragWorldNormal;
layout(location = 2) out vec3 fragColor;

void main()
{
	fragWorldPos = vec3(MMatrix.Model * vec4(vertexPosition, 1.0));
	gl_Position = VPMatrix.Projection * VPMatrix.View * vec4(fragWorldPos, 0.0);
	fragWorldNormal = vec3(MMatrix.Model * vec4(vertexNormal, 1.0));
	fragColor = vertexColor;
}