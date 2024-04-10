#include "Mesh.h"

ave::Mesh::Mesh(const vk::Device& device, const vk::PhysicalDevice& physicalDevice)
	: m_Device{ device }
	, m_PhysicalDevice{ physicalDevice }
{	
}

ave::Mesh::~Mesh()
{
	m_Device.destroyBuffer(m_Buffer.Buffer);
	m_Device.freeMemory(m_Buffer.BufferMemory);
}

void ave::Mesh::InitializeBuffer()
{
	vkUtil::BufferInBundle inBundle{};
	inBundle.Device = m_Device;
	inBundle.PhysicalDevice = m_PhysicalDevice;
	inBundle.Size = sizeof(vkUtil::Vertex2D) * m_VertexVec.size();
	inBundle.UsageFlags = vk::BufferUsageFlagBits::eVertexBuffer;

	m_Buffer = vkUtil::CreateBuffer(inBundle);

	void* memoryLocation{ m_Device.mapMemory(m_Buffer.BufferMemory, 0, inBundle.Size) };
	memcpy(memoryLocation, m_VertexVec.data(), inBundle.Size);
	m_Device.unmapMemory(m_Buffer.BufferMemory);
}

void ave::Mesh::AddVertex(const vkUtil::Vertex2D& vertex)
{
	m_VertexVec.emplace_back(vertex);
}

void ave::Mesh::BindBuffer(const vk::CommandBuffer& commandBuffer)
{
	vk::Buffer vertexBufferArr[]{ m_Buffer.Buffer };
	vk::DeviceSize offsetArr[]{ 0 };
	commandBuffer.bindVertexBuffers(0, 1, vertexBufferArr, offsetArr);
}

void ave::Mesh::Draw(const vk::CommandBuffer& commandBuffer) const
{
	commandBuffer.draw(m_VertexVec.size(), 1, 0, 0);
}
