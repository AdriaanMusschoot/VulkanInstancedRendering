#include "Frame.h"
#include "Rendering/Image.h"
#include <execution>

void vkUtil::SwapchainFrame::CreateDescriptorResources(std::int64_t const& nrWorldMatrices)
{
	BufferInBundle inputUBO;
	inputUBO.Device = Device;
	inputUBO.PhysicalDevice = PhysicalDevice;
	inputUBO.MemoryPropertyFlags = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
	inputUBO.Size = sizeof(UBO);
	inputUBO.UsageFlags = vk::BufferUsageFlagBits::eUniformBuffer;

	VPBuffer = vkUtil::CreateBuffer(inputUBO);
	VPWriteLocationPtr = Device.mapMemory(VPBuffer.BufferMemory, 0, inputUBO.Size);

	UBODescriptorInfo.buffer = VPBuffer.Buffer;
	UBODescriptorInfo.offset = 0;
	UBODescriptorInfo.range = sizeof(UBO);

	BufferInBundle inputStorage;
	inputStorage.Device = Device;
	inputStorage.PhysicalDevice = PhysicalDevice;
	inputStorage.MemoryPropertyFlags = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
	//TODO make input parameter out of 1069
	inputStorage.Size = nrWorldMatrices * sizeof(glm::mat4);
	inputStorage.UsageFlags = vk::BufferUsageFlagBits::eStorageBuffer;

	WBuffer = vkUtil::CreateBuffer(inputStorage);
	WBufferWriteLocationPtr = Device.mapMemory(WBuffer.BufferMemory, 0, inputStorage.Size);

	WMatrixVec.resize(inputStorage.Size, glm::mat4(1.0f));

	WDescriptorInfo.buffer = WBuffer.Buffer;
	WDescriptorInfo.offset = 0;
	WDescriptorInfo.range = inputStorage.Size;
}

void vkUtil::SwapchainFrame::WriteDescriptorSet()
{
	vk::WriteDescriptorSet writeInfoUBO{};
	writeInfoUBO.dstSet = DescriptorSet;
	writeInfoUBO.dstBinding = 0;
	writeInfoUBO.dstArrayElement = 0;
	writeInfoUBO.descriptorCount = 1;
	writeInfoUBO.descriptorType = vk::DescriptorType::eUniformBuffer;
	writeInfoUBO.pBufferInfo = &UBODescriptorInfo;

	Device.updateDescriptorSets(writeInfoUBO, nullptr);

	vk::WriteDescriptorSet writeInfoStorage{};
	writeInfoStorage.dstSet = DescriptorSet;
	writeInfoStorage.dstBinding = 1;
	writeInfoStorage.dstArrayElement = 0;
	writeInfoStorage.descriptorCount = 1;
	writeInfoStorage.descriptorType = vk::DescriptorType::eStorageBuffer;
	writeInfoStorage.pBufferInfo = &WDescriptorInfo;

	Device.updateDescriptorSets(writeInfoStorage, nullptr);
}

void vkUtil::SwapchainFrame::CreateDepthResources()
{
	std::vector<vk::Format> formatVec{};
	formatVec.emplace_back(vk::Format::eD32Sfloat);
	formatVec.emplace_back(vk::Format::eD24UnormS8Uint);
	DepthFormat = vkInit::GetSupportedFormat
	(
		PhysicalDevice,
		formatVec,
		vk::ImageTiling::eOptimal,
		vk::FormatFeatureFlagBits::eDepthStencilAttachment
	);

	vkInit::ImageInBundle imgInput;
	imgInput.Device = Device;
	imgInput.PhysicalDevice = PhysicalDevice;
	imgInput.Tiling = vk::ImageTiling::eOptimal;
	imgInput.UsageFlags = vk::ImageUsageFlagBits::eDepthStencilAttachment;
	imgInput.MemoryPropertyFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;
	imgInput.Extent = DepthExtent;
	imgInput.Format = DepthFormat;
	DepthBuffer = vkInit::CreateImage(imgInput);
	DepthBufferMemory = vkInit::CreateImageMemory(imgInput, DepthBuffer);
	DepthBufferView = vkInit::CreateImageView
	(
		Device, DepthBuffer, DepthFormat, vk::ImageAspectFlagBits::eDepth
	);
}

void vkUtil::SwapchainFrame::Destroy()
{
	Device.destroyImage(DepthBuffer);
	Device.freeMemory(DepthBufferMemory);
	Device.destroyImageView(DepthBufferView);
	Device.destroySemaphore(SemaphoreRenderingFinished);
	Device.destroySemaphore(SemaphoreImageAvailable);
	Device.destroyFence(InFlightFence);
	Device.destroyFramebuffer(Framebuffer);
	Device.destroyImageView(ImageView);
	Device.unmapMemory(VPBuffer.BufferMemory);
	Device.freeMemory(VPBuffer.BufferMemory);
	Device.destroyBuffer(VPBuffer.Buffer);
	Device.unmapMemory(WBuffer.BufferMemory);
	Device.freeMemory(WBuffer.BufferMemory);
	Device.destroyBuffer(WBuffer.Buffer);
}
