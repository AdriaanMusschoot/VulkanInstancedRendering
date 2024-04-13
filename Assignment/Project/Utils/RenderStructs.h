#ifndef VK_RENDERSTRUCTS_H
#define VK_RENDERSTRUCTS_H
#include "Engine/Configuration.h"

namespace vkUtil
{
	struct Vertex2D
	{
		glm::vec2 Position;
		glm::vec3 Color;
	};
	std::vector<vk::VertexInputBindingDescription> GetBindingDescription2D();
	std::vector<vk::VertexInputAttributeDescription> GetAttributeDescription2D();

	struct Vertex3D
	{
		glm::vec3 Position;
		glm::vec3 Color;
		glm::vec3 Normal;
	};
	std::vector<vk::VertexInputBindingDescription> GetBindingDescription3D();
	std::vector<vk::VertexInputAttributeDescription> GetAttributeDescription3D();
}

#endif