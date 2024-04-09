#ifndef VK_PIPELINE_H
#define VK_PIPELINE_H
#include "Engine/Configuration.h"
#include "Shader.h"
#include "Utils/RenderStructs.h"

namespace vkInit
{

	struct GraphicsPipelineInBundle
	{
		vk::Device Device;
		std::string	VertexFilePath;
		std::string FragmentFilePath;
		vk::Extent2D SwapchainExtent;
		vk::Format SwapchainImgFormat;
	};
	
	struct GraphicsPipelineOutBundle
	{
		vk::PipelineLayout Layout;
		vk::RenderPass RenderPass;
		vk::Pipeline Pipeline;
	};

	vk::PipelineLayout CreatePipelineLayout(const vk::Device& device, bool isDebugging)
	{
		vk::PushConstantRange pushConstantRange{};
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(vkUtil::ObjectData);
		pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eVertex;

		vk::PipelineLayoutCreateInfo layoutCreateInfo{};
		layoutCreateInfo.flags = vk::PipelineLayoutCreateFlags{};
		layoutCreateInfo.setLayoutCount = 0;
		layoutCreateInfo.pushConstantRangeCount = 1;
		layoutCreateInfo.pPushConstantRanges = &pushConstantRange;
			
		try
		{
			return device.createPipelineLayout(layoutCreateInfo);
		}
		catch (const vk::SystemError& systemError)
		{
			if (isDebugging)
			{
				std::cout << systemError.what() << "\n";
			}
			return nullptr;
		}
	}

	vk::RenderPass CreateRenderPass(const vk::Device& device, const vk::Format& swapchainImgFormat, bool isDebugging)
	{
		vk::AttachmentDescription colorAttachmentDescription{};
		colorAttachmentDescription.flags = vk::AttachmentDescriptionFlags{};
		colorAttachmentDescription.format = swapchainImgFormat;
		colorAttachmentDescription.samples = vk::SampleCountFlagBits::e1;
		colorAttachmentDescription.loadOp = vk::AttachmentLoadOp::eClear;
		colorAttachmentDescription.storeOp = vk::AttachmentStoreOp::eStore;
		colorAttachmentDescription.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
		colorAttachmentDescription.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		colorAttachmentDescription.initialLayout = vk::ImageLayout::eUndefined;
		colorAttachmentDescription.finalLayout = vk::ImageLayout::ePresentSrcKHR;

		vk::AttachmentReference colorAttachmentReference{};
		colorAttachmentReference.attachment = 0;
		colorAttachmentReference.layout = vk::ImageLayout::eColorAttachmentOptimal;

		vk::SubpassDescription subpass{};
		subpass.flags = vk::SubpassDescriptionFlags{};
		subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentReference;

		vk::RenderPassCreateInfo renderPassCreateInfo{};
		renderPassCreateInfo.flags = vk::RenderPassCreateFlags{};
		renderPassCreateInfo.attachmentCount = 1;
		renderPassCreateInfo.pAttachments = &colorAttachmentDescription;
		renderPassCreateInfo.subpassCount = 1;
		renderPassCreateInfo.pSubpasses = &subpass;

		try
		{
			return device.createRenderPass(renderPassCreateInfo);
		}
		catch (const vk::SystemError& systemError)
		{
			if (isDebugging)
			{
				std::cout << systemError.what() << "\n";
			}
			return nullptr;
		}
	}

	GraphicsPipelineOutBundle CreateGraphicsPipeline(const GraphicsPipelineInBundle& in, bool isDebugging)
	{
		if (isDebugging)
		{
			std::cout << "\nPipeline creation started\n";
		}

		vk::GraphicsPipelineCreateInfo pipelineCreateInfo{};
		pipelineCreateInfo.flags = vk::PipelineCreateFlags{};

		std::vector<vk::PipelineShaderStageCreateInfo> shaderStageCreateInfoVec{};

		//Vertex input/what we will be sending
		vk::VertexInputBindingDescription bindingDescription{ vkUtil::GetPosColBindingDescription() };
		std::array<vk::VertexInputAttributeDescription, 2> attributeDescriptionArr{ vkUtil::GetPosColAttributeDescription() };

		vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
		vertexInputStateCreateInfo.flags = vk::PipelineVertexInputStateCreateFlags{};
		vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
		vertexInputStateCreateInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 2;
		vertexInputStateCreateInfo.pVertexAttributeDescriptions = attributeDescriptionArr.data();

		pipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;

		//Input assembly/how should thy interpreteth it
		vk::PipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo{};
		inputAssemblyCreateInfo.flags = vk::PipelineInputAssemblyStateCreateFlags{};
		inputAssemblyCreateInfo.topology = vk::PrimitiveTopology::eTriangleList;
		
		pipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;

		if (isDebugging)
		{
			std::cout << "\tShader module creation started\n";
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

		pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStageCreateInfoVec.size());
		pipelineCreateInfo.pStages = shaderStageCreateInfoVec.data();

		if (isDebugging)
		{
			std::cout << "\tViewport creation started\n";
		}

		vk::Viewport viewport{};
		viewport.x = 0;
		viewport.y = 0;
		viewport.width = static_cast<float>(in.SwapchainExtent.width);
		viewport.height = static_cast<float>(in.SwapchainExtent.height);
		viewport.minDepth = 0.0f;
		viewport.minDepth = 1.0f;

		if (isDebugging)
		{
			std::cout << "\tScissor creation started\n";
		}

		vk::Rect2D scissor{};
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		scissor.extent = in.SwapchainExtent;

		vk::PipelineViewportStateCreateInfo viewportStateCreateInfo{};
		viewportStateCreateInfo.flags = vk::PipelineViewportStateCreateFlags{};
		viewportStateCreateInfo.viewportCount = 1;
		viewportStateCreateInfo.pViewports = &viewport;
		viewportStateCreateInfo.scissorCount = 1;
		viewportStateCreateInfo.pScissors = &scissor;
		
		pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;

		if (isDebugging)
		{
			std::cout << "\tRasterizer creation started\n";
		}

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

		if (isDebugging)
		{
			std::cout << "\tMultisample creation started\n";
		}

		vk::PipelineMultisampleStateCreateInfo multisampleStateCreateInfo{};
		multisampleStateCreateInfo.flags = vk::PipelineMultisampleStateCreateFlags{};
		multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
		multisampleStateCreateInfo.rasterizationSamples = vk::SampleCountFlagBits::e1;

		pipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;

		if (isDebugging)
		{
			std::cout << "\tColor blend creation started\n";
		}

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

		if (isDebugging)
		{
			std::cout << "\tPipeline layout creation started\n";
		}

		vk::PipelineLayout pipelineLayout{ CreatePipelineLayout(in.Device, isDebugging) };

		pipelineCreateInfo.layout = pipelineLayout;

		if (isDebugging)
		{
			std::cout << "\tRenderpass creation started\n";
		}

		vk::RenderPass renderPass{ CreateRenderPass(in.Device, in.SwapchainImgFormat, isDebugging) };

		pipelineCreateInfo.renderPass = renderPass;

		pipelineCreateInfo.basePipelineHandle = nullptr;

		if (isDebugging)
		{
			std::cout << "Pipeline creation ended\n";
		}

		vk::Pipeline pipeline{};

		try
		{
			pipeline = in.Device.createGraphicsPipeline(nullptr, pipelineCreateInfo).value;
		}
		catch (const vk::SystemError& systemError)
		{
			if (isDebugging)
			{
				std::cout << systemError.what() << "\n";
			}
		}

		GraphicsPipelineOutBundle out{};
		out.Layout = pipelineLayout;
		out.RenderPass = renderPass;
		out.Pipeline = pipeline;

		in.Device.destroyShaderModule(vertexShaderModule);
		in.Device.destroyShaderModule(fragmentShaderModule);
		return out;
	}

}

#endif