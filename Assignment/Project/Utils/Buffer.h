#ifndef VK_BUFFER_H
#define VK_BUFFER_H
#include "Engine/Configuration.h"

namespace vkUtil
{
	struct BufferInBundle
	{
		size_t Size{ static_cast<size_t>(-1) };
		vk::BufferUsageFlags UsageFlags;
		vk::Device Device;
		vk::PhysicalDevice PhysicalDevice;
		vk::MemoryPropertyFlags MemoryPropertyFlags;
	};

	struct DataBuffer
	{
		vk::Buffer Buffer;
		vk::DeviceMemory BufferMemory;
	};

	uint32_t FindMemoryTypeIndex(const vk::PhysicalDevice& physicalDevice, uint32_t supportedMemoryIndices, vk::MemoryPropertyFlags requestedProperties);

	void AllocateBufferMemory(DataBuffer& buffer, const BufferInBundle& in);

	DataBuffer CreateBuffer(const BufferInBundle& in);

	void CopyBuffer(DataBuffer& srcBuffer, DataBuffer& dstBuffer, const vk::DeviceSize& size, const vk::Queue& queue, const vk::CommandBuffer& commandBuffer);

}

#endif
