#version 450

layout(location = 0) in vec3 fragWorldPosition;
layout(location = 1) in vec3 fragWorldNormal;
layout(location = 2) in vec2 fragTexCoor;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform sampler2D material;

struct Light
{
	vec3 direction;
	vec3 color;
	float intensity;
};

void main()
{
	float ambientLight = 0.25f;
	Light mainLight;
	mainLight.direction = vec3(0.577f, -0.577f, -0.577f);
	mainLight.color = vec3(1.0f, 1.0f, 1.0);
	mainLight.intensity = 10.0f;
	
	float cosAngle = max(dot(fragWorldNormal, normalize(-mainLight.direction)), 0);
	outColor = cosAngle * texture(material, fragTexCoor);
}