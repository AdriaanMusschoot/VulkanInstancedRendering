#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "vulkan/vulkan.h"
#include "GP2CommandPool.h"
#include "GP2DataBuffer.h"

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
	
		void InitializeVertexBuffers(VkQueue graphicsQueue, CommandPool& commandPool);
		void Draw(const VkCommandBuffer& commandBuffer) const;
		
		Mesh(VkPhysicalDevice physicalDevice, VkDevice device);

		void AddVertex(Vertex&& vertex);
		void AddIndex(int&& index);
	private:
		std::vector<Vertex> m_VertexVec{};
		std::vector<uint16_t> m_IndexVec{};

		std::unique_ptr<DataBuffer> m_VertexBuffer;
		std::unique_ptr<DataBuffer> m_IndexBuffer;

		VkDevice m_VkDevice;
		VkPhysicalDevice m_VkPhysicalDevice;
	};

}