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
		std::string FragmentFilePath;
		vk::Extent2D SwapchainExtent;
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
			std::cout << "Shader module creation started\n";
		}

		vk::ShaderModule vertexShaderModule{ vkUtil::CreateModule(in.Device, in.VertexFilePath, isDebugging) };

		vk::PipelineShaderStageCreateInfo vertexShaderStageCreateInfo{};
		vertexShaderStageCreateInfo.flags = vk::PipelineShaderStageCreateFlags{};
		vertexShaderStageCreateInfo.stage = vk::ShaderStageFlagBits::eVertex;
		vertexShaderStageCreateInfo.module = vertexShaderModule;
		vertexShaderStageCreateInfo.pName = "main";

		vk::ShaderModule fragmentShaderModule{ vkUtil::CreateModule(in.Device, in.FragmentFilePath, isDebugging) };

		vk::PipelineShaderStageCreateInfo fragmentShaderStageCreateInfo{};
		fragmentShaderStageCreateInfo.flags = vk::PipelineShaderStageCreateFlags{};
		fragmentShaderStageCreateInfo.stage = vk::ShaderStageFlagBits::eFragment;
		fragmentShaderStageCreateInfo.module = fragmentShaderModule;
		fragmentShaderStageCreateInfo.pName = "main";

		shaderStageCreateInfoVec.emplace_back(vertexShaderStageCreateInfo);
		shaderStageCreateInfoVec.emplace_back(fragmentShaderStageCreateInfo);

		pipelineCreateInfo.stageCount = shaderStageCreateInfoVec.size();
		pipelineCreateInfo.pStages = shaderStageCreateInfoVec.data();

		vk::Viewport viewport{};
		viewport.x = 0;
		viewport.y = 0;
		viewport.width = in.SwapchainExtent.width;
		viewport.height = in.SwapchainExtent.height;
		viewport.minDepth = 0.0f;
		viewport.minDepth = 1.0f;

		vk::Rect2D scissor{};
		scissor.offset.x = 0.0f;
		scissor.offset.y = 0.0f;
		scissor.extent = in.SwapchainExtent;

		vk::PipelineViewportStateCreateInfo viewportStateCreateInfo{};
		viewportStateCreateInfo.flags = vk::PipelineViewportStateCreateFlags{};
		viewportStateCreateInfo.viewportCount = 1;
		viewportStateCreateInfo.pViewports = &viewport;
		viewportStateCreateInfo.scissorCount = 1;
		viewportStateCreateInfo.pScissors = &scissor;
		
		pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;

		vk::PipelineRasterizationStateCreateInfo rasterizerStateCreateInfo{};
		rasterizerStateCreateInfo.flags = vk::PipelineRasterizationStateCreateFlags{};
		rasterizerStateCreateInfo.depthClampEnable = VK_FALSE;
		rasterizerStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
		rasterizerStateCreateInfo.polygonMode = vk::PolygonMode::eFill;
		rasterizerStateCreateInfo.lineWidth = 1.0f;
		rasterizerStateCreateInfo.cullMode = vk::CullModeFlagBits::eNone;
		rasterizerStateCreateInfo.frontFace = vk::FrontFace::eClockwise;
		rasterizerStateCreateInfo.depthBiasEnable = VK_FALSE;

		pipelineCreateInfo.pRasterizationState = &rasterizerStateCreateInfo;

		vk::PipelineMultisampleStateCreateInfo multisampleStateCreateInfo{};
		multisampleStateCreateInfo.flags = vk::PipelineMultisampleStateCreateFlags{};
		multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
		multisampleStateCreateInfo.rasterizationSamples = vk::SampleCountFlagBits::e1;

		pipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;

		vk::PipelineColorBlendAttachmentState colorBlendAttachmentState{};
		colorBlendAttachmentState.colorWriteMask =	vk::ColorComponentFlagBits::eR | 
													vk::ColorComponentFlagBits::eG | 
													vk::ColorComponentFlagBits::eB | 
													vk::ColorComponentFlagBits::eA;
		colorBlendAttachmentState.blendEnable = VK_FALSE;

		vk::PipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{};
		colorBlendStateCreateInfo.flags = vk::PipelineColorBlendStateCreateFlags{};
		colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
		colorBlendStateCreateInfo.logicOp = vk::LogicOp::eCopy;
		colorBlendStateCreateInfo.attachmentCount = 1;
		colorBlendStateCreateInfo.pAttachments = &colorBlendAttachmentState;
		colorBlendStateCreateInfo.blendConstants[0] = 0.0f;
		colorBlendStateCreateInfo.blendConstants[1] = 0.0f;
		colorBlendStateCreateInfo.blendConstants[2] = 0.0f;
		colorBlendStateCreateInfo.blendConstants[3] = 0.0f;

		pipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;

		GraphicsPipelineOutBundle out{};

		in.Device.destroyShaderModule(vertexShaderModule);
		in.Device.destroyShaderModule(fragmentShaderModule);
		return out;
	}

}

#endif