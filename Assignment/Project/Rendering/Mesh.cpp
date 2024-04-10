#include "Mesh.h"

ave::Mesh::Mesh(const vk::Device& device, const vk::PhysicalDevice& physicalDevice)
	: m_Device{ device }
	, m_PhysicalDevice{ physicalDevice }
{	
}

ave::Mesh::~Mesh()
{
	m_Device.destroyBuffer(m_VertexBuffer.Buffer);
	m_Device.freeMemory(m_VertexBuffer.BufferMemory);
	m_Device.destroyBuffer(m_IndexBuffer.Buffer);
	m_Device.freeMemory(m_IndexBuffer.BufferMemory);
}

void ave::Mesh::AddVertex(const vkUtil::Vertex2D& vertex)
{
	m_VertexVec.emplace_back(vertex);
}

void ave::Mesh::InitializeVertexBuffer(const MeshInBundle& in)
{
	vkUtil::BufferInBundle inBundle{};
	inBundle.Device = m_Device;
	inBundle.PhysicalDevice = m_PhysicalDevice;
	inBundle.Size = sizeof(vkUtil::Vertex2D) * m_VertexVec.size();
	inBundle.UsageFlags = vk::BufferUsageFlagBits::eTransferSrc;
	inBundle.MemoryPropertyFlags = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

	vkUtil::DataBuffer stagingBuffer = vkUtil::CreateBuffer(inBundle);

	void* memoryLocation{ m_Device.mapMemory(stagingBuffer.BufferMemory, 0, inBundle.Size) };
	memcpy(memoryLocation, m_VertexVec.data(), inBundle.Size);
	m_Device.unmapMemory(stagingBuffer.BufferMemory);

	inBundle.UsageFlags = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer;
	inBundle.MemoryPropertyFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;
	m_VertexBuffer = vkUtil::CreateBuffer(inBundle);

	vkUtil::CopyBuffer(stagingBuffer, m_VertexBuffer, inBundle.Size, in.GraphicsQueue, in.MainCommandBuffer);

	m_Device.destroyBuffer(stagingBuffer.Buffer);
	m_Device.freeMemory(stagingBuffer.BufferMemory);
}

void ave::Mesh::AddIndex(uint32_t idx)
{
	m_IndexVec.emplace_back(idx);
}

void ave::Mesh::InitializeIndexBuffer(const MeshInBundle& in)
{
	vkUtil::BufferInBundle inBundle{};
	inBundle.Device = m_Device;
	inBundle.PhysicalDevice = m_PhysicalDevice;
	inBundle.Size = sizeof(int) * m_IndexVec.size();
	inBundle.UsageFlags = vk::BufferUsageFlagBits::eTransferSrc;
	inBundle.MemoryPropertyFlags = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

	vkUtil::DataBuffer stagingBuffer = vkUtil::CreateBuffer(inBundle);

	void* memoryLocation{ m_Device.mapMemory(stagingBuffer.BufferMemory, 0, inBundle.Size) };
	memcpy(memoryLocation, m_IndexVec.data(), inBundle.Size);
	m_Device.unmapMemory(stagingBuffer.BufferMemory);

	inBundle.UsageFlags = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer;
	inBundle.MemoryPropertyFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;
	m_IndexBuffer = vkUtil::CreateBuffer(inBundle);

	vkUtil::CopyBuffer(stagingBuffer, m_IndexBuffer, inBundle.Size, in.GraphicsQueue, in.MainCommandBuffer);

	m_Device.destroyBuffer(stagingBuffer.Buffer);
	m_Device.freeMemory(stagingBuffer.BufferMemory);
}

void ave::Mesh::BindBuffers(const vk::CommandBuffer& commandBuffer)
{
	vk::Buffer vertexBufferArr[]{ m_VertexBuffer.Buffer };
	vk::DeviceSize offsetArr[]{ 0 };
	commandBuffer.bindVertexBuffers(0, 1, vertexBufferArr, offsetArr);
	commandBuffer.bindIndexBuffer(m_IndexBuffer.Buffer, 0, vk::IndexType::eUint32);
}

void ave::Mesh::Draw(const vk::CommandBuffer& commandBuffer) const
{
	commandBuffer.drawIndexed(static_cast<uint32_t>(m_IndexVec.size()), 1, 0, 0, 0);
}