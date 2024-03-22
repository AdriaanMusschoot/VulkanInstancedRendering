#include "GP2Mesh.h"
#include <stdexcept>

void amu::Mesh::Initialize(VkPhysicalDevice physicalDevice, VkDevice device)
{	
	m_VkPhysicalDevice = physicalDevice;
	m_VkDevice = device;
}

void amu::Mesh::InitializeVertexBuffers(VkQueue graphicsQueue, CommandPool& commandPool)
{
	VkDeviceSize bufferSize = sizeof(m_VertexVec[0]) * m_VertexVec.size();
	
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
	VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
	stagingBuffer, stagingBufferMemory);
	
	 void* data;
	vkMapMemory(m_VkDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
	 memcpy(data, m_VertexVec.data(), (size_t)bufferSize);
	 vkUnmapMemory(m_VkDevice, stagingBufferMemory);
	
	CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT |
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			m_VkVertexBuffer, m_VkVertexBufferMemory);

	CopyBuffer(stagingBuffer, m_VkVertexBuffer, bufferSize, graphicsQueue, commandPool);

	vkDestroyBuffer(m_VkDevice, stagingBuffer, nullptr);
	vkFreeMemory(m_VkDevice, stagingBufferMemory, nullptr);
}

void amu::Mesh::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkQueue graphicsQueue, CommandPool& commandPool) 
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool.GetCommandPool();
	allocInfo.commandBufferCount = 1;
	
	 VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(m_VkDevice, &allocInfo, &commandBuffer);
	
	VkCommandBufferBeginInfo beginInfo{};
	 beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	 beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	
	vkBeginCommandBuffer(commandBuffer, &beginInfo);
	
	VkBufferCopy copyRegion{};
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
	
	vkEndCommandBuffer(commandBuffer);
	
	VkSubmitInfo submitInfo{};
	 submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	 submitInfo.commandBufferCount = 1;
	 submitInfo.pCommandBuffers = &commandBuffer;
	
	vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	 vkQueueWaitIdle(graphicsQueue);
	
	vkFreeCommandBuffers(m_VkDevice, commandPool.GetCommandPool(), 1, &commandBuffer);	
}


void amu::Mesh::Draw(const VkCommandBuffer& commandBuffer) const
{
	VkBuffer vertexBuffers[] = { m_VkVertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdDraw(commandBuffer, m_VertexVec.size(), 1, 0, 0);
}

void amu::Mesh::Destroy()
{
	vkDestroyBuffer(m_VkDevice, m_VkVertexBuffer, nullptr);
	vkFreeMemory(m_VkDevice, m_VkVertexBufferMemory, nullptr);
}

void amu::Mesh::AddVertex(Vertex&& vertex)
{
	m_VertexVec.emplace_back(std::move(vertex));
}

void amu::Mesh::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(m_VkDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to create buffer!");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(m_VkDevice, buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryTypes(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(m_VkDevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate buffer memory!");
		}

		vkBindBufferMemory(m_VkDevice, buffer, bufferMemory, 0);
}

int amu::Mesh::FindMemoryTypes(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{	
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(m_VkPhysicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}
