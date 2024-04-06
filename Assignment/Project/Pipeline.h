#ifndef VK_PIPELINE_H
#define VK_PIPELINE_H
#include "Configuration.h"
#include "Shader.h"

namespace vkInit
{

	struct GraphicsPipelineInBundle
	{
		vk::Device Device;
		std::string	VertexFilePath;
	};
	
	struct GraphicsPipelineOutBundle
	{

	};

	GraphicsPipelineOutBundle CreateGraphicsPipeline(const GraphicsPipelineInBundle& in, bool isDebugging)
	{
		vk::GraphicsPipelineCreateInfo pipelineCreateInfo{};
		pipelineCreateInfo.flags = vk::PipelineCreateFlags{};

		std::vector<vk::PipelineShaderStageCreateInfo> shaderStageCreateInfoVec{};

		//Vertex input/what we will be sending
		vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
		vertexInputStateCreateInfo.flags = vk::PipelineVertexInputStateCreateFlags{};
		vertexInputStateCreateInfo.vertexBindingDescriptionCount = 0;
		vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 0;

		pipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;

		//Input assembly/how should thy interpreteth it
		vk::PipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo{};
		inputAssemblyCreateInfo.flags = vk::PipelineInputAssemblyStateCreateFlags{};
		inputAssemblyCreateInfo.topology = vk::PrimitiveTopology::eTriangleList;
		
		pipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;

		if (isDebugging)
		{
			std::cout << "Shader module creation:\n";
		}

		vk::ShaderModule vertexShaderModule{ vkUtil::CreateModule(in.Device, in.VertexFilePath, isDebugging) };

		vk::PipelineShaderStageCreateInfo vertexShaderStageCreateInfo{};
		vertexShaderStageCreateInfo.flags = vk::PipelineShaderStageCreateFlags{};
		vertexShaderStageCreateInfo.stage = vk::ShaderStageFlagBits::eVertex;
		vertexShaderStageCreateInfo.module = vertexShaderModule;
		vertexShaderStageCreateInfo.pName = "main";
		shaderStageCreateInfoVec.emplace_back(vertexShaderStageCreateInfo);

		GraphicsPipelineOutBundle out{};

		return out;
	}

}

#endif