#include "Image.h"
#include "Utils/Buffer.h"
#include "Pipeline/Descriptor.h"
#define STB_IMAGE_IMPLEMENTATION
#include "Utils/STBI.h"

vkInit::Texture::Texture(const TextureInBundle& texIn)
	: m_Device{ texIn.Device }
	, m_PhysicalDevice{ texIn.PhysicalDevice }
	, m_FileName{ texIn.FileName }
	, m_CommandBuffer{ texIn.CommandBuffer }
	, m_Queue{ texIn.Queue }
	, m_DescriptorSetLayout{ texIn.DescriptorSetLayout }
	, m_DescriptorPool{ texIn.DescriptorPool }
{
	m_Pixels = stbi_load(m_FileName.c_str(), &m_Width, &m_Height, &m_Channels, STBI_rgb_alpha);
	
	ImageInBundle imageInBundle{};
	imageInBundle.Device = m_Device;
	imageInBundle.PhysicalDevice = m_PhysicalDevice;
	imageInBundle.Extent = vk::Extent2D{ static_cast<uint32_t>(m_Width), static_cast<uint32_t>(m_Height) };
	imageInBundle.Tiling = vk::ImageTiling::eOptimal;
	imageInBundle.UsageFlags = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
	imageInBundle.MemoryPropertyFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;
	imageInBundle.Format = vk::Format::eR8G8B8A8Unorm;
	
	m_Image = CreateImage(imageInBundle);
	
	m_ImageMemory = CreateImageMemory(imageInBundle, m_Image);
	
	Populate();
	
	//stbi function
	free(m_Pixels);
	
	m_ImageView = CreateImageView(m_Device, m_Image, vk::Format::eR8G8B8A8Unorm, vk::ImageAspectFlagBits::eColor);
	
	CreateSampler();
	
	CreateDescriptorSet();
}

vkInit::Texture::~Texture()
{
	m_Device.freeMemory(m_ImageMemory);
	m_Device.destroyImage(m_Image);
	m_Device.destroyImageView(m_ImageView);
	m_Device.destroySampler(m_Sampler);
}

void vkInit::Texture::Apply(const vk::CommandBuffer& commandBuffer, const vk::PipelineLayout& pipelineLayout)
{
	commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 1, m_DescriptorSet, nullptr);
}

void vkInit::Texture::Populate()
{
	vkUtil::BufferInBundle bufferInBundle{};
	bufferInBundle.Device = m_Device;
	bufferInBundle.PhysicalDevice = m_PhysicalDevice;
	bufferInBundle.MemoryPropertyFlags = vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible;
	bufferInBundle.UsageFlags = vk::BufferUsageFlagBits::eTransferSrc;
	bufferInBundle.Size = m_Width * m_Height * 4; //multiply by the size of the bites per pixel (uint32 -> 4 bytes)

	vkUtil::DataBuffer stagingBuffer = vkUtil::CreateBuffer(bufferInBundle);

	void* writeLocationPtr{ m_Device.mapMemory(stagingBuffer.BufferMemory, 0, bufferInBundle.Size) };
	memcpy(writeLocationPtr, m_Pixels, bufferInBundle.Size);
	m_Device.unmapMemory(stagingBuffer.BufferMemory);

	ImageLayoutTransitionInBundle transitionInBundle{};
	transitionInBundle.CommandBuffer = m_CommandBuffer;
	transitionInBundle.Queue = m_Queue;
	transitionInBundle.Image = m_Image;
	transitionInBundle.OldLayout = vk::ImageLayout::eUndefined;
	transitionInBundle.NewLayout = vk::ImageLayout::eTransferDstOptimal;
	TransitionImageLayout(transitionInBundle);

	BufferCopyImageInBundle copyInBundle{};
	copyInBundle.CommandBuffer = m_CommandBuffer;
	copyInBundle.Queue = m_Queue;
	copyInBundle.SourceBuffer = stagingBuffer.Buffer;
	copyInBundle.DestinationImage = m_Image;
	copyInBundle.Extent = vk::Extent2D{ static_cast<uint32_t>(m_Width), static_cast<uint32_t>(m_Height) };
	CopyBufferToImage(copyInBundle);


	transitionInBundle.OldLayout = vk::ImageLayout::eTransferDstOptimal;
	transitionInBundle.NewLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
	TransitionImageLayout(transitionInBundle);

	m_Device.freeMemory(stagingBuffer.BufferMemory);
	m_Device.destroyBuffer(stagingBuffer.Buffer);
}

void vkInit::Texture::CreateSampler()
{
	vk::SamplerCreateInfo samplerCreateInfo{};
	samplerCreateInfo.flags = vk::SamplerCreateFlags{};
	samplerCreateInfo.minFilter = vk::Filter::eNearest;
	samplerCreateInfo.magFilter = vk::Filter::eLinear;
	samplerCreateInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
	samplerCreateInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
	samplerCreateInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
	samplerCreateInfo.anisotropyEnable = vk::False;
	samplerCreateInfo.maxAnisotropy = 1.0f;
	samplerCreateInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
	samplerCreateInfo.unnormalizedCoordinates = vk::False;
	samplerCreateInfo.compareEnable = vk::False;
	samplerCreateInfo.compareOp = vk::CompareOp::eAlways;
	samplerCreateInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
	samplerCreateInfo.mipLodBias = 0.0f;
	samplerCreateInfo.minLod = 0.0f;
	samplerCreateInfo.maxLod = 0.0f;

	try
	{
		m_Sampler = m_Device.createSampler(samplerCreateInfo);
	}
	catch (const vk::SystemError& systemError)
	{
		std::cout << systemError.what() << "\n";
	}
}

void vkInit::Texture::CreateDescriptorSet()
{
	m_DescriptorSet = vkInit::CreateDescriptorSet(m_Device, m_DescriptorPool, m_DescriptorSetLayout);

	vk::DescriptorImageInfo descriptorImageInfo{};
	descriptorImageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
	descriptorImageInfo.imageView = m_ImageView;
	descriptorImageInfo.sampler = m_Sampler;

	vk::WriteDescriptorSet des{};
	des.dstSet = m_DescriptorSet;
	des.dstBinding = 0;
	des.dstArrayElement = 0;
	des.descriptorType = vk::DescriptorType::eCombinedImageSampler;
	des.descriptorCount = 1;
	des.pImageInfo = &descriptorImageInfo;

	m_Device.updateDescriptorSets(des, nullptr);
}

vk::Image vkInit::CreateImage(const ImageInBundle& in)
{
	vk::ImageCreateInfo imgCreateInfo{};
	imgCreateInfo.flags = vk::ImageCreateFlags{};
	imgCreateInfo.imageType = vk::ImageType::e2D;
	imgCreateInfo.extent = vk::Extent3D{ in.Extent, 1 };
	imgCreateInfo.mipLevels = 1;
	imgCreateInfo.arrayLayers = 1;
	imgCreateInfo.format = in.Format;
	imgCreateInfo.tiling = in.Tiling;
	imgCreateInfo.initialLayout = vk::ImageLayout::eUndefined;
	imgCreateInfo.usage = in.UsageFlags;
	imgCreateInfo.sharingMode = vk::SharingMode::eExclusive;
	imgCreateInfo.samples = vk::SampleCountFlagBits::e1;

	try
	{
		return in.Device.createImage(imgCreateInfo);
	}
	catch (const vk::SystemError& systemError)
	{
		
		std::cout << systemError.what() << "\n";
		
		return nullptr;
	}
}

vk::DeviceMemory vkInit::CreateImageMemory(const ImageInBundle& in, const vk::Image& image)
{
	vk::MemoryRequirements memoryRequirements{ in.Device.getImageMemoryRequirements(image) };

	vk::MemoryAllocateInfo memAllocateInfo{};
	memAllocateInfo.allocationSize = memoryRequirements.size;
	memAllocateInfo.memoryTypeIndex = vkUtil::FindMemoryTypeIndex(in.PhysicalDevice, memoryRequirements.memoryTypeBits, in.MemoryPropertyFlags);

	try
	{
		vk::DeviceMemory imgMemory{ in.Device.allocateMemory(memAllocateInfo) };
		in.Device.bindImageMemory(image, imgMemory, 0);
		return imgMemory;
	}
	catch (const vk::SystemError& systemError)
	{
		
		std::cout << systemError.what() << "\n";
		
		return nullptr;
	}

}

void vkInit::TransitionImageLayout(const ImageLayoutTransitionInBundle& in)
{
	vkInit::BeginSingleCommand(in.CommandBuffer);

	vk::ImageSubresourceRange imgSubRange{};
	imgSubRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	imgSubRange.baseMipLevel = 0;
	imgSubRange.levelCount = 1;
	imgSubRange.baseArrayLayer = 0;
	imgSubRange.layerCount = 1;

	vk::ImageMemoryBarrier barrier;
	barrier.oldLayout = in.OldLayout;
	barrier.newLayout = in.NewLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = in.Image;
	barrier.subresourceRange = imgSubRange;

	vk::PipelineStageFlags sourceStageFlags;
	vk::PipelineStageFlags destinationStageFlags;
	if (in.OldLayout == vk::ImageLayout::eUndefined
		and in.NewLayout == vk::ImageLayout::eTransferDstOptimal)
	{
		barrier.srcAccessMask = vk::AccessFlagBits::eNoneKHR;
		barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

		//flag is before the pipeline starts
		sourceStageFlags = vk::PipelineStageFlagBits::eTopOfPipe;
		destinationStageFlags = vk::PipelineStageFlagBits::eTransfer;
	}
	else
	{
		barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

		//flag is before the pipeline starts
		sourceStageFlags = vk::PipelineStageFlagBits::eTransfer;
		destinationStageFlags = vk::PipelineStageFlagBits::eFragmentShader;
	}

	in.CommandBuffer.pipelineBarrier
	(
		sourceStageFlags,
		destinationStageFlags,
		vk::DependencyFlags{},
		nullptr,
		nullptr,
		barrier
	);

	vkInit::EndSingleCommand(in.CommandBuffer, in.Queue);
}

void vkInit::CopyBufferToImage(const BufferCopyImageInBundle& in)
{
	vkInit::BeginSingleCommand(in.CommandBuffer);

	vk::BufferImageCopy copy;
	copy.bufferOffset = 0;
	copy.bufferRowLength = 0;
	copy.bufferImageHeight = 0;

	vk::ImageSubresourceLayers imgSubLayers;
	imgSubLayers.aspectMask = vk::ImageAspectFlagBits::eColor;
	imgSubLayers.mipLevel = 0;
	imgSubLayers.baseArrayLayer = 0;
	imgSubLayers.layerCount = 1;
	
	copy.imageSubresource = imgSubLayers;

	copy.imageOffset = vk::Offset3D{ 0, 0, 0 };
	copy.imageExtent = vk::Extent3D{ in.Extent, 1 };

	in.CommandBuffer.copyBufferToImage
	(
		in.SourceBuffer,
		in.DestinationImage,
		vk::ImageLayout::eTransferDstOptimal,
		copy
	);

	vkInit::EndSingleCommand(in.CommandBuffer, in.Queue);
}

vk::ImageView vkInit::CreateImageView(const vk::Device& device, const vk::Image& image, const vk::Format& format, const vk::ImageAspectFlags& aspectFlags)
{
	vk::ImageViewCreateInfo imgViewCreateInfo{};
	imgViewCreateInfo.image = image;
	imgViewCreateInfo.format = format;
	imgViewCreateInfo.viewType = vk::ImageViewType::e2D;
	imgViewCreateInfo.components.r = vk::ComponentSwizzle::eIdentity;
	imgViewCreateInfo.components.g = vk::ComponentSwizzle::eIdentity;
	imgViewCreateInfo.components.b = vk::ComponentSwizzle::eIdentity;
	imgViewCreateInfo.components.a = vk::ComponentSwizzle::eIdentity;
	imgViewCreateInfo.subresourceRange.aspectMask = aspectFlags;
	imgViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imgViewCreateInfo.subresourceRange.levelCount = 1;
	imgViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imgViewCreateInfo.subresourceRange.layerCount = 1;

	return device.createImageView(imgViewCreateInfo);
}

vk::Format vkInit::GetSupportedFormat(const vk::PhysicalDevice& physicalDevice, const std::vector<vk::Format>& formatVec, const vk::ImageTiling& tiling, const vk::FormatFeatureFlags& featureFlags)
{
	for (const auto& format : formatVec)
	{
		vk::FormatProperties properties{ physicalDevice.getFormatProperties(format) };

		if (tiling == vk::ImageTiling::eLinear and
			(properties.linearTilingFeatures & featureFlags) == featureFlags)
		{
			return format;
		}
		else if (tiling == vk::ImageTiling::eOptimal and
			(properties.optimalTilingFeatures & featureFlags) == featureFlags)
		{
			return format; 
		}
	}
	return formatVec[0];
}
