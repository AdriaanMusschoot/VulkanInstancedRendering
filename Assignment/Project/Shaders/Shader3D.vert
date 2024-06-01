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

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTexCoor;

layout(location = 0) out vec3 fragWorldPosition;
layout(location = 1) out vec3 fragWorldNormal;
layout(location = 2) out vec2 fragTexCoor;

void main()
{
	fragWorldPosition = vec3(WorldMatrix.Model[gl_InstanceIndex] * vec4(vertexPosition, 1.0));
	gl_Position = VPMatrix.Projection * VPMatrix.View * vec4(fragWorldPosition, 1.0);
	fragWorldNormal = normalize(normalize(vertexNormal) * mat3(WorldMatrix.Model[gl_InstanceIndex]));
	fragTexCoor = vertexTexCoor;
}