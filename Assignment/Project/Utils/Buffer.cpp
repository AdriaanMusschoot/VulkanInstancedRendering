#include "Buffer.h"

uint32_t vkUtil::FindMemoryTypeIndex(const vk::PhysicalDevice& physicalDevice, uint32_t supportedMemoryIndices, vk::MemoryPropertyFlags requestedProperties)
{
	vk::PhysicalDeviceMemoryProperties memoryProperties = physicalDevice.getMemoryProperties();

	for (uint32_t idx{}; idx < memoryProperties.memoryTypeCount; idx++)
	{
		bool supported{ static_cast<bool>(supportedMemoryIndices & (1 << idx)) };

		bool sufficient{ (memoryProperties.memoryTypes[idx].propertyFlags & requestedProperties) == requestedProperties };

		if (supported and sufficient)
		{
			return idx;
		}
	}

	return 0;
}

void vkUtil::AllocateBufferMemory(DataBuffer& buffer, const BufferInBundle& in)
{
	vk::MemoryRequirements memoryRequirements{ in.Device.getBufferMemoryRequirements(buffer.Buffer) };

	vk::MemoryAllocateInfo allocateInfo{};
	allocateInfo.allocationSize = memoryRequirements.size;
	allocateInfo.memoryTypeIndex = FindMemoryTypeIndex
	(
		in.PhysicalDevice,
		memoryRequirements.memoryTypeBits,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
	);

	buffer.BufferMemory = in.Device.allocateMemory(allocateInfo);
	in.Device.bindBufferMemory(buffer.Buffer, buffer.BufferMemory, 0);
}

vkUtil::DataBuffer vkUtil::CreateBuffer(const BufferInBundle& in)
{
	vk::BufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.flags = vk::BufferCreateFlags{};
	bufferCreateInfo.size = in.Size;
	bufferCreateInfo.usage = in.UsageFlags;
	bufferCreateInfo.sharingMode = vk::SharingMode::eExclusive;

	DataBuffer buffer;
	buffer.Buffer = in.Device.createBuffer(bufferCreateInfo);

	AllocateBufferMemory(buffer, in);

	return buffer;
}
