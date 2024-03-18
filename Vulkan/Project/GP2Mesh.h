#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "vulkan/vulkan.h"

namespace amu
{

	class Mesh
	{
	public:
		struct Vertex
		{
			glm::vec2 pos;
			glm::vec3 color;
	
			static VkVertexInputBindingDescription getBindingDescription()
			{
				VkVertexInputBindingDescription bindingDescription{};
				bindingDescription.binding = 0;
				bindingDescription.stride = sizeof(Vertex);
				bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	
				return bindingDescription;
			}
	
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions()
			{
				std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
	
				attributeDescriptions.resize(2);
	
				attributeDescriptions[0].binding = 0;
				attributeDescriptions[0].location = 0;
				attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
				attributeDescriptions[0].offset = offsetof(Vertex, pos);
	
				attributeDescriptions[1].binding = 0;
				attributeDescriptions[1].location = 1;
				attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
				attributeDescriptions[1].offset = offsetof(Vertex, color);
	
				return attributeDescriptions;
			}
		};
	
		void Initialize(VkPhysicalDevice physicalDevice, VkDevice device);
		void InitializeVertexBuffers();
		void Draw(const VkCommandBuffer& commandBuffer) const;
		
		void Destroy();

		void AddVertex(Vertex&& vertex);

		void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		
		VkBuffer m_StagingBuffer;
		VkDeviceMemory m_StagingBufferMemory;
		
	private:
		std::vector<Vertex> m_VertexVec{};

		VkBuffer m_VertexBuffer;
		VkDeviceMemory m_VertexBufferMemory;
		VkDevice m_Device;
		VkPhysicalDevice m_PhysicalDevice;

		int FindMemoryTypes(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	};

}