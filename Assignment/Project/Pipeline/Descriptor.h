#ifndef VK_DESCRIPTOR_H
#define VK_DESCRIPTOR_H
#include "Engine/Configuration.h"

namespace vkInit
{
	struct DescriptorSetLayoutData
	{
		int Count{ -1 };
		std::vector<int> IndexVec;
		std::vector<vk::DescriptorType> TypeVec;
		std::vector<int> CountVec;
		std::vector<vk::ShaderStageFlags> StageFlagVec;
	};

	vk::DescriptorSetLayout CreateDescriptorSetLayout(const vk::Device& device, const DescriptorSetLayoutData& layoutData);

	vk::DescriptorPool CreateDescriptorPool(const vk::Device& device, uint32_t size, const DescriptorSetLayoutData& layoutData);

	vk::DescriptorSet CreateDescriptorSet(const vk::Device& device, const vk::DescriptorPool& descriptorPool, const vk::DescriptorSetLayout& setLayout);;

}

#endif