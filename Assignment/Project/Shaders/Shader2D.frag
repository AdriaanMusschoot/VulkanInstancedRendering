#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoor;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform sampler2D material;

void main()
{
    outColor = texture(material, fragTexCoor);
}