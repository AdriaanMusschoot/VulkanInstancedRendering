#include "Descriptor.h"

vk::DescriptorSetLayout vkInit::CreateDescriptorSetLayout(const vk::Device& device, const DescriptorSetLayoutData& layoutData)
{
	std::vector<vk::DescriptorSetLayoutBinding> layoutBindingVec;
	layoutBindingVec.reserve(layoutData.Count);

	for (int idx{}; idx < layoutData.Count; ++idx)
	{
		vk::DescriptorSetLayoutBinding setLayoutBinding;
		setLayoutBinding.binding = layoutData.IndexVec[idx];
		setLayoutBinding.descriptorType = layoutData.TypeVec[idx];
		setLayoutBinding.descriptorCount = layoutData.CountVec[idx];
		setLayoutBinding.stageFlags = layoutData.StageFlagVec[idx];

		layoutBindingVec.emplace_back(setLayoutBinding);
	}

	vk::DescriptorSetLayoutCreateInfo setLayoutCreateInfo{};
	setLayoutCreateInfo.flags = vk::DescriptorSetLayoutCreateFlags{};
	setLayoutCreateInfo.bindingCount = layoutData.Count;
	setLayoutCreateInfo.pBindings = layoutBindingVec.data();

	try
	{
		return device.createDescriptorSetLayout(setLayoutCreateInfo);
	}
	catch (const vk::SystemError& systemError)
	{
		std::cout << systemError.what() << "\n";

		return nullptr;
	}
}

vk::DescriptorPool vkInit::CreateDescriptorPool(const vk::Device& device, uint32_t size, const DescriptorSetLayoutData& layoutData)
{
	std::vector<vk::DescriptorPoolSize> poolSizeVec;

	for (int idx{}; idx < layoutData.Count; ++idx)
	{
		vk::DescriptorPoolSize poolSize;
		poolSize.type = layoutData.TypeVec[idx];
		poolSize.descriptorCount = size;
		poolSizeVec.emplace_back(poolSize);
	}

	vk::DescriptorPoolCreateInfo poolCreateInfo{};
	poolCreateInfo.flags = vk::DescriptorPoolCreateFlags{};
	poolCreateInfo.maxSets = size;
	poolCreateInfo.poolSizeCount = static_cast<uint32_t>(poolSizeVec.size());
	poolCreateInfo.pPoolSizes = poolSizeVec.data();

	try
	{
		return device.createDescriptorPool(poolCreateInfo);
	}
	catch (const vk::SystemError& systemError)
	{
		std::cout << systemError.what() << "\n";

		return nullptr;
	}
}

vk::DescriptorSet vkInit::CreateDescriptorSet(const vk::Device& device, const vk::DescriptorPool& descriptorPool, const vk::DescriptorSetLayout& setLayout)
{
	vk::DescriptorSetAllocateInfo setAllocateInfo{};
	setAllocateInfo.descriptorPool = descriptorPool;
	setAllocateInfo.descriptorSetCount = 1;
	setAllocateInfo.pSetLayouts = &setLayout;

	try
	{
		return device.allocateDescriptorSets(setAllocateInfo)[0];
	}
	catch (const vk::SystemError& systemError)
	{

		std::cout << systemError.what() << "\n";

		return nullptr;
	}
}
