#include "Mesh.h"

ave::Mesh::Mesh(const vk::Device& device, const vk::PhysicalDevice& physicalDevice)
	: m_Device{ device }
{
	std::vector<float> vertices = { {
		 0.0f, -0.05f, 0.0f, 1.0f, 0.0f,
		 0.05f, 0.05f, 0.0f, 1.0f, 0.0f,
		-0.05f, 0.05f, 0.0f, 1.0f, 0.0f
	} };

	vkUtil::BufferInBundle inBundle{};
	inBundle.Device = device;
	inBundle.PhysicalDevice = physicalDevice;
	inBundle.Size = sizeof(float) * vertices.size();
	inBundle.UsageFlags = vk::BufferUsageFlagBits::eVertexBuffer;

	m_Buffer = vkUtil::CreateBuffer(inBundle);

	void* memoryLocation{ m_Device.mapMemory(m_Buffer.BufferMemory, 0, inBundle.Size) };
	memcpy(memoryLocation, vertices.data(), inBundle.Size);
	m_Device.unmapMemory(m_Buffer.BufferMemory);
}

ave::Mesh::~Mesh()
{
	m_Device.destroyBuffer(m_Buffer.Buffer);
	m_Device.freeMemory(m_Buffer.BufferMemory);
}

const vkUtil::DataBuffer& ave::Mesh::GetBuffer() const
{
	return m_Buffer;
}
