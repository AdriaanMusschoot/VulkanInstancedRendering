#include "GP2Mesh.h"
#include <stdexcept>

void amu::Mesh::Initialize(VkPhysicalDevice physicalDevice, VkDevice device)
{	
	m_PhysicalDevice = physicalDevice;
	m_Device = device;

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = sizeof(m_VertexVec[0]) * m_VertexVec.size();
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(m_Device, &bufferInfo, nullptr, &m_Buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create vertex buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(m_Device, m_Buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryTypes(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	if (vkAllocateMemory(m_Device, &allocInfo, nullptr, &m_BufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate vertex buffer memory!");
	}

	vkBindBufferMemory(m_Device, m_Buffer, m_BufferMemory, 0);

	void* data;
	vkMapMemory(device, m_BufferMemory, 0, bufferInfo.size, 0, &data);
	memcpy(data, m_VertexVec.data(), (size_t)bufferInfo.size);
	vkUnmapMemory(device, m_BufferMemory);
}

void amu::Mesh::Draw(const VkCommandBuffer& commandBuffer) const
{
	VkBuffer vertexBuffers[] = { m_Buffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdDraw(commandBuffer, m_VertexVec.size(), 1, 0, 0);
}

void amu::Mesh::Destroy()
{
	vkDestroyBuffer(m_Device, m_Buffer, nullptr);
	vkFreeMemory(m_Device, m_BufferMemory, nullptr);
}

void amu::Mesh::AddVertex(Vertex&& vertex)
{
	m_VertexVec.emplace_back(std::move(vertex));
}

int amu::Mesh::FindMemoryTypes(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{	
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}
