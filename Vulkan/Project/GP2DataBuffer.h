#pragma once
#include "vulkanbase/VulkanUtil.h"
#include "GP2CommandPool.h"
namespace amu
{

	class DataBuffer
	{
	public:
		DataBuffer(VkDevice device, VkPhysicalDevice physicalDevice);
		~DataBuffer();
		void InitializeBuffer(void* bufferData, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDeviceSize size, VkQueue graphicsQueue, CommandPool& commandPool);

		void BindAsVertexBuffer(VkCommandBuffer commandBuffer);
		void BindAsIndexBuffer(VkCommandBuffer commandBuffer);
	private:
		VkDevice m_VkDevice;
		VkPhysicalDevice m_VkPhysicalDevice;

		VkBuffer m_VkBuffer;
		VkDeviceMemory m_VkBufferMemory;
		VkDeviceSize m_Size;
	
		void CreateBuffer(VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		int FindMemoryTypes(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		void CopyBuffer(VkBuffer srcBuffer, VkQueue graphicsQueue, CommandPool& commandPool);
	};

}