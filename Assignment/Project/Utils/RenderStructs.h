#ifndef VK_RENDERSTRUCTS_H
#define VK_RENDERSTRUCTS_H
#include "Engine/Configuration.h"

namespace vkUtil
{
	template<typename T>
	concept Vertex = requires(T t)
	{
		{ t.GetBindingDescription() } -> std::same_as<std::vector<vk::VertexInputBindingDescription>>;
		{ t.GetAttributeDescription() } -> std::same_as<std::vector<vk::VertexInputAttributeDescription>>;
	};

	enum AttachmentFlags
	{
		Color = 0b0001,
		Depth = 0b0010
	};

	struct MeshInBundle
	{
		vk::Queue const& GraphicsQueue;
		vk::CommandBuffer const& MainCommandBuffer;
		vk::Device const& Device;
		vk::PhysicalDevice const& PhysicalDevice;
	};

	struct Vertex2D
	{
		glm::vec2 Position;
		glm::vec2 TextureCoordinate;
		static std::vector<vk::VertexInputBindingDescription> GetBindingDescription();
		static std::vector<vk::VertexInputAttributeDescription> GetAttributeDescription();
	};

	struct Vertex3D
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 UV;
		static std::vector<vk::VertexInputBindingDescription> GetBindingDescription();
		static std::vector<vk::VertexInputAttributeDescription> GetAttributeDescription();
	};
}

#endif