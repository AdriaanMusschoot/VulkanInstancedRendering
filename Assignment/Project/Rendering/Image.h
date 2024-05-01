#ifndef VK_IMAGE_H
#define VK_IMAGE_H

#include "Engine/Configuration.h"
#include "Utils/Buffer.h"
#include "Rendering/Commands.h"

namespace vkInit
{
	struct TextureInBundle
	{
		vk::Device Device;
		vk::PhysicalDevice PhysicalDevice;
		std::string FileName;
		vk::CommandBuffer CommandBuffer;
		vk::Queue Queue;
		vk::DescriptorSetLayout DescriptorSetLayout;
		vk::DescriptorPool DescriptorPool;
	};


	struct ImageInBundle
	{
		vk::Device Device;
		vk::PhysicalDevice PhysicalDevice;
		vk::Extent2D Extent;
		vk::ImageTiling Tiling;
		vk::ImageUsageFlags UsageFlags;
		vk::MemoryPropertyFlags MemoryPropertyFlags;
		vk::Format Format;
	};

	struct ImageLayoutTransitionInBundle
	{
		vk::CommandBuffer CommandBuffer;
		vk::Queue Queue;
		vk::Image Image;
		vk::ImageLayout OldLayout;
		vk::ImageLayout NewLayout;
	};

	struct BufferCopyImageInBundle
	{
		vk::CommandBuffer CommandBuffer;
		vk::Queue Queue;
		vk::Buffer SourceBuffer;
		vk::Image DestinationImage;
		vk::Extent2D Extent;
	};

	class Texture final
	{
	public:
		Texture(const TextureInBundle& texIn);
		~Texture();

		Texture(const Texture& other) = delete;
		Texture(Texture&& other) = delete;
		Texture& operator=(const Texture& other) = delete;
		Texture& operator=(Texture&& other) = delete;

		void Apply(const vk::CommandBuffer& commandBuffer, const vk::PipelineLayout& pipelineLayout);
	private:
		int m_Width{ 0 };
		int m_Height{ 0 };
		int m_Channels{ 0 };
		vk::Device m_Device;
		vk::PhysicalDevice m_PhysicalDevice;
		std::string m_FileName;
		unsigned char* m_Pixels;

		vk::Image m_Image;
		vk::ImageView m_ImageView;
		vk::DeviceMemory m_ImageMemory;
		vk::Sampler m_Sampler;

		vk::DescriptorSetLayout m_DescriptorSetLayout;
		vk::DescriptorSet m_DescriptorSet;
		vk::DescriptorPool m_DescriptorPool;

		vk::CommandBuffer m_CommandBuffer;
		vk::Queue m_Queue;

		void Populate();
		void CreateSampler();
		void CreateDescriptorSet();
	};

	vk::Image CreateImage(const ImageInBundle& in);

	vk::DeviceMemory CreateImageMemory(const ImageInBundle& in, const vk::Image& image);

	void TransitionImageLayout(const ImageLayoutTransitionInBundle& in);

	void CopyBufferToImage(const BufferCopyImageInBundle& in);

	vk::ImageView CreateImageView(const vk::Device& device, const vk::Image& image, const vk::Format& format, const vk::ImageAspectFlags& aspectFlags);

	vk::Format GetSupportedFormat(const vk::PhysicalDevice& physicalDevice, const std::vector<vk::Format>& formatVec, const vk::ImageTiling& tiling, const vk::FormatFeatureFlags& featureFlags);
}

#endif