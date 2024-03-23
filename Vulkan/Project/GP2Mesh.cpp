#include "GP2Mesh.h"
#include <stdexcept>

amu::Mesh::Mesh(VkPhysicalDevice physicalDevice, VkDevice device)
	: m_VkPhysicalDevice{ physicalDevice }
	, m_VkDevice{ device }
{
}

void amu::Mesh::InitializeVertexBuffers(VkQueue graphicsQueue, CommandPool& commandPool)
{
	m_IndexBuffer = std::make_unique<DataBuffer>(m_VkDevice, m_VkPhysicalDevice);
	m_VertexBuffer = std::make_unique<DataBuffer>(m_VkDevice, m_VkPhysicalDevice);
	
	m_IndexBuffer->InitializeBuffer(
		m_IndexVec.data(),
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		sizeof(m_IndexVec[0]) * m_IndexVec.size(),
		graphicsQueue, 
		commandPool);
	
	m_VertexBuffer->InitializeBuffer(
		m_VertexVec.data(),
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		sizeof(m_VertexVec[0]) * m_VertexVec.size(),
		graphicsQueue,
		commandPool);
}

void amu::Mesh::Draw(const VkCommandBuffer& commandBuffer) const
{
	m_VertexBuffer->BindAsVertexBuffer(commandBuffer);
	m_IndexBuffer->BindAsIndexBuffer(commandBuffer);
	vkCmdDrawIndexed(commandBuffer,	static_cast<uint32_t>(m_IndexVec.size()), 1, 0, 0, 0);
}

void amu::Mesh::AddVertex(Vertex&& vertex)
{
	m_VertexVec.emplace_back(std::move(vertex));
}

void amu::Mesh::AddIndex(int&& index)
{
	m_IndexVec.emplace_back(std::move(index));
}