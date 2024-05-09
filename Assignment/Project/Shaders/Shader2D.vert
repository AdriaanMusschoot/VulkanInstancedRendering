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

//std140 enforc that layout on cpu is same as on gpu
layout(std140, binding = 1) readonly buffer StorageBuffer
{
	mat4 Model[];
} WorldMatrix;

layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in vec2 vertexTexCoord;

layout(location = 0) out vec2 fragTexCoord;

void main()
{
	gl_Position = WorldMatrix.Model[gl_InstanceIndex] * vec4(vertexPosition, 0.9, 1.0);
	fragTexCoord = vertexTexCoord;
}