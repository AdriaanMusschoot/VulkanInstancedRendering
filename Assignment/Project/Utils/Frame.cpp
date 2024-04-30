#include "Frame.h"
#include "Rendering/Image.h"

void vkUtil::SwapchainFrame::CreateUBOResources()
{
	BufferInBundle input;
	input.Device = Device;
	input.PhysicalDevice = PhysicalDevice;
	input.MemoryPropertyFlags = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
	input.Size = sizeof(UBO);
	input.UsageFlags = vk::BufferUsageFlagBits::eUniformBuffer;

	VPBuffer = vkUtil::CreateBuffer(input);
	VPWriteLocationPtr = Device.mapMemory(VPBuffer.BufferMemory, 0, sizeof(UBO));

	UBODescriptorInfo.buffer = VPBuffer.Buffer;
	UBODescriptorInfo.offset = 0;
	UBODescriptorInfo.range = sizeof(UBO);
}

void vkUtil::SwapchainFrame::WriteDescriptorSet()
{
	vk::WriteDescriptorSet writeInfo{};
	writeInfo.dstSet = UBODescriptorSet;
	writeInfo.dstBinding = 0;
	writeInfo.dstArrayElement = 0;
	writeInfo.descriptorCount = 1;
	writeInfo.descriptorType = vk::DescriptorType::eUniformBuffer;
	writeInfo.pBufferInfo = &UBODescriptorInfo;

	Device.updateDescriptorSets(writeInfo, nullptr);
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
}
