#ifndef VK_PIPELINE_H
#define VK_PIPELINE_H
#include "Engine/Configuration.h"
#include "Shader.h"
#include "Utils/RenderStructs.h"
#include "Rendering/Scene.h"
#include "RenderPass.h"
#include "functional"

namespace vkInit
{

	template <typename VertexStruct>
	class Pipeline final
	{
	public:

		struct GraphicsPipelineInBundle
		{
			vk::Device Device;
			std::string	VertexFilePath;
			std::string FragmentFilePath;
			vk::Extent2D SwapchainExtent;
			vk::RenderPass RenderPass;
			vk::DescriptorSetLayout DescriptorSetLayout;
			std::function<std::vector<vk::VertexInputBindingDescription>()> GetBindingDescription;
			std::function<std::vector<vk::VertexInputAttributeDescription>()> GetAttributeDescription;
		};

		struct GraphicsPipelineOutBundle
		{
			vk::PipelineLayout Layout;
			vk::Pipeline Pipeline;
		};

		Pipeline(const GraphicsPipelineInBundle& in, bool isDebugging)
			: m_Device{ in.Device }
		{
			GraphicsPipelineOutBundle out = CreateGraphicsPipeline(in, isDebugging);
			m_PipelineLayout = out.Layout;
			m_Pipeline = out.Pipeline;
		}
		~Pipeline()
		{
			m_Device.destroyPipeline(m_Pipeline);
			m_Device.destroyPipelineLayout(m_PipelineLayout);
		}

		Pipeline(const Pipeline& other) = delete;
		Pipeline(Pipeline&& other) = delete;
		Pipeline& operator=(const Pipeline& other) = delete;
		Pipeline& operator=(Pipeline&& other) = delete;

		void Record(const vk::CommandBuffer& commandBuffer, const vk::Framebuffer& frameBuffer, const vk::Extent2D& swapchainExtent, const vk::DescriptorSet& descriptorSet)
		{
			commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_PipelineLayout, 0, descriptorSet, nullptr);

			commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_Pipeline);

			if (m_SceneUPtr)
			{
				m_SceneUPtr->Draw(commandBuffer, m_PipelineLayout);
			}
		}
		
		void SetScene(std::unique_ptr<ave::Scene<VertexStruct>> sceneUPtr)
		{
			m_SceneUPtr = std::move(sceneUPtr);
		}
	private:
		vk::PipelineLayout m_PipelineLayout;
		vk::Pipeline m_Pipeline;

		vk::Device m_Device;

		std::unique_ptr<ave::Scene<VertexStruct>> m_SceneUPtr;

		vk::PipelineLayout CreatePipelineLayout(const vk::Device& device, const vk::DescriptorSetLayout& descriptorSetLayout, bool isDebugging)
		{
			vk::PushConstantRange pushConstantRange{};
			pushConstantRange.offset = 0;
			pushConstantRange.size = sizeof(glm::mat4);
			pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eVertex;

			vk::PipelineLayoutCreateInfo layoutCreateInfo{};
			layoutCreateInfo.flags = vk::PipelineLayoutCreateFlags{};
			layoutCreateInfo.setLayoutCount = 1;
			layoutCreateInfo.pSetLayouts = &descriptorSetLayout;
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
		vk::PipelineVertexInputStateCreateInfo PopulateVertexInput(const std::vector<vk::VertexInputBindingDescription>& bindingDescriptionVec, const std::vector<vk::VertexInputAttributeDescription>& attributeDescriptionVec)
		{
			vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
			vertexInputStateCreateInfo.flags = vk::PipelineVertexInputStateCreateFlags{};
			vertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptionVec.size());
			vertexInputStateCreateInfo.pVertexBindingDescriptions = bindingDescriptionVec.data();
			vertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptionVec.size());
			vertexInputStateCreateInfo.pVertexAttributeDescriptions = attributeDescriptionVec.data();

			return vertexInputStateCreateInfo;
		}
		vk::PipelineInputAssemblyStateCreateInfo PopulateInputAssembly()
		{

			vk::PipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo{};
			inputAssemblyCreateInfo.flags = vk::PipelineInputAssemblyStateCreateFlags{};
			inputAssemblyCreateInfo.topology = vk::PrimitiveTopology::eTriangleList;

			return inputAssemblyCreateInfo;
		}
		vk::PipelineShaderStageCreateInfo PopulateShaderStage(const vk::ShaderModule& shaderModule, const vk::ShaderStageFlagBits& flagBits)
		{
			vk::PipelineShaderStageCreateInfo shaderStageCreateInfo{};
			shaderStageCreateInfo.flags = vk::PipelineShaderStageCreateFlags{};
			shaderStageCreateInfo.stage = flagBits;
			shaderStageCreateInfo.module = shaderModule;
			shaderStageCreateInfo.pName = "main";

			return shaderStageCreateInfo;
		}
		vk::PipelineDepthStencilStateCreateInfo PopulateDepthState()
		{
			vk::PipelineDepthStencilStateCreateInfo depthStateCreateInfo{};
			depthStateCreateInfo.flags = vk::PipelineDepthStencilStateCreateFlags{};
			depthStateCreateInfo.depthTestEnable = VK_TRUE;
			depthStateCreateInfo.depthWriteEnable = VK_TRUE;
			depthStateCreateInfo.depthCompareOp = vk::CompareOp::eLess;
			depthStateCreateInfo.minDepthBounds = 0.0f;
			depthStateCreateInfo.maxDepthBounds = 1.f;
			depthStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
			depthStateCreateInfo.stencilTestEnable = VK_FALSE;
			depthStateCreateInfo.front = vk::StencilOpState{};
			depthStateCreateInfo.back = vk::StencilOpState{};

			return depthStateCreateInfo;
		}
		vk::PipelineViewportStateCreateInfo PopulateViewportState(const vk::Viewport& viewport, const vk::Rect2D& scissor)
		{
			vk::PipelineViewportStateCreateInfo viewportStateCreateInfo{};
			viewportStateCreateInfo.flags = vk::PipelineViewportStateCreateFlags{};
			viewportStateCreateInfo.viewportCount = 1;
			viewportStateCreateInfo.pViewports = &viewport;
			viewportStateCreateInfo.scissorCount = 1;
			viewportStateCreateInfo.pScissors = &scissor;

			return viewportStateCreateInfo;
		}
		vk::PipelineRasterizationStateCreateInfo PopulateRasterizationState()
		{
			vk::PipelineRasterizationStateCreateInfo rasterizerStateCreateInfo{};
			rasterizerStateCreateInfo.flags = vk::PipelineRasterizationStateCreateFlags{};
			rasterizerStateCreateInfo.depthClampEnable = vk::False;
			rasterizerStateCreateInfo.rasterizerDiscardEnable = vk::False;
			rasterizerStateCreateInfo.polygonMode = vk::PolygonMode::eFill;
			rasterizerStateCreateInfo.lineWidth = 1.0f;
			rasterizerStateCreateInfo.cullMode = vk::CullModeFlagBits::eBack;
			rasterizerStateCreateInfo.frontFace = vk::FrontFace::eClockwise;
			rasterizerStateCreateInfo.depthBiasEnable = vk::False;

			return rasterizerStateCreateInfo;
		}
		vk::PipelineMultisampleStateCreateInfo PopulateMultisampleState()
		{
			vk::PipelineMultisampleStateCreateInfo multisampleStateCreateInfo{};
			multisampleStateCreateInfo.flags = vk::PipelineMultisampleStateCreateFlags{};
			multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
			multisampleStateCreateInfo.rasterizationSamples = vk::SampleCountFlagBits::e1;
			multisampleStateCreateInfo.minSampleShading = 0.0f;
			multisampleStateCreateInfo.pSampleMask = nullptr;
			multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
			multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;
			return multisampleStateCreateInfo;
		}
		vk::PipelineColorBlendAttachmentState PopulateColorBlendAttachmentState()
		{
			vk::PipelineColorBlendAttachmentState colorBlendAttachmentState{};
			colorBlendAttachmentState.colorWriteMask = vk::ColorComponentFlagBits::eR |
				vk::ColorComponentFlagBits::eG |
				vk::ColorComponentFlagBits::eB |
				vk::ColorComponentFlagBits::eA;
			colorBlendAttachmentState.blendEnable = VK_FALSE;
			colorBlendAttachmentState.srcColorBlendFactor = vk::BlendFactor::eOne;
			colorBlendAttachmentState.dstColorBlendFactor = vk::BlendFactor::eZero;
			colorBlendAttachmentState.colorBlendOp = vk::BlendOp::eAdd;
			colorBlendAttachmentState.srcAlphaBlendFactor = vk::BlendFactor::eOne;
			colorBlendAttachmentState.dstAlphaBlendFactor = vk::BlendFactor::eZero;
			colorBlendAttachmentState.alphaBlendOp = vk::BlendOp::eAdd;

			return colorBlendAttachmentState;
		}
		vk::PipelineColorBlendStateCreateInfo PopulateColorBlendState(const vk::PipelineColorBlendAttachmentState& colorBlendAttachment)
		{
			vk::PipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{};
			colorBlendStateCreateInfo.flags = vk::PipelineColorBlendStateCreateFlags{};
			colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
			colorBlendStateCreateInfo.logicOp = vk::LogicOp::eCopy;
			colorBlendStateCreateInfo.attachmentCount = 1;
			colorBlendStateCreateInfo.pAttachments = &colorBlendAttachment;
			colorBlendStateCreateInfo.blendConstants[0] = 0.0f;
			colorBlendStateCreateInfo.blendConstants[1] = 0.0f;
			colorBlendStateCreateInfo.blendConstants[2] = 0.0f;
			colorBlendStateCreateInfo.blendConstants[3] = 0.0f;

			return colorBlendStateCreateInfo;
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
			std::vector<vk::VertexInputBindingDescription> bindingDescription{ in.GetBindingDescription() };
			std::vector attributeDescriptionArr{ in.GetAttributeDescription() };

			vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{ PopulateVertexInput(bindingDescription, attributeDescriptionArr) };
			pipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;

			//Input assembly/how should thy interpreteth it
			vk::PipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo{ PopulateInputAssembly() };
			pipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;

			if (isDebugging)
			{
				std::cout << "\tShader module creation started\n";
			}

			vk::ShaderModule vertexShaderModule{ vkUtil::CreateModule(in.Device, in.VertexFilePath, isDebugging) };
			vk::PipelineShaderStageCreateInfo vertexShaderStageCreateInfo{ PopulateShaderStage(vertexShaderModule, vk::ShaderStageFlagBits::eVertex) };

			vk::ShaderModule fragmentShaderModule{ vkUtil::CreateModule(in.Device, in.FragmentFilePath, isDebugging) };
			vk::PipelineShaderStageCreateInfo fragmentShaderStageCreateInfo{ PopulateShaderStage(fragmentShaderModule, vk::ShaderStageFlagBits::eFragment) };

			shaderStageCreateInfoVec.emplace_back(vertexShaderStageCreateInfo);
			shaderStageCreateInfoVec.emplace_back(fragmentShaderStageCreateInfo);

			pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStageCreateInfoVec.size());
			pipelineCreateInfo.pStages = shaderStageCreateInfoVec.data();

			if (isDebugging)
			{
				std::cout << "\tDepth creation started\n";
			}

			vk::PipelineDepthStencilStateCreateInfo depthStateCreateInfo{ PopulateDepthState() };
			pipelineCreateInfo.pDepthStencilState = &depthStateCreateInfo;

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
			viewport.maxDepth = 1.f;

			if (isDebugging)
			{
				std::cout << "\tScissor creation started\n";
			}

			vk::Rect2D scissor{};
			scissor.offset.x = 0;
			scissor.offset.y = 0;
			scissor.extent = in.SwapchainExtent;

			vk::PipelineViewportStateCreateInfo viewportStateCreateInfo{ PopulateViewportState(viewport, scissor) };
			pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;

			if (isDebugging)
			{
				std::cout << "\tRasterizer creation started\n";
			}

			vk::PipelineRasterizationStateCreateInfo rasterizerStateCreateInfo{ PopulateRasterizationState() };
			pipelineCreateInfo.pRasterizationState = &rasterizerStateCreateInfo;

			if (isDebugging)
			{
				std::cout << "\tMultisample creation started\n";
			}

			vk::PipelineMultisampleStateCreateInfo multisampleStateCreateInfo{ PopulateMultisampleState() };
			pipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;

			if (isDebugging)
			{
				std::cout << "\tColor blend creation started\n";
			}

			vk::PipelineColorBlendAttachmentState colorBlendAttachmentState{ PopulateColorBlendAttachmentState() };
			vk::PipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{ PopulateColorBlendState(colorBlendAttachmentState) };
			pipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;

			if (isDebugging)
			{
				std::cout << "\tPipeline layout creation started\n";
			}

			vk::PipelineLayout pipelineLayout{ CreatePipelineLayout(in.Device, in.DescriptorSetLayout, isDebugging) };
			pipelineCreateInfo.layout = pipelineLayout;

			if (isDebugging)
			{
				std::cout << "\tRenderpass creation started\n";
			}

			pipelineCreateInfo.renderPass = in.RenderPass;

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
			out.Pipeline = pipeline;

			in.Device.destroyShaderModule(vertexShaderModule);
			in.Device.destroyShaderModule(fragmentShaderModule);
			return out;
		}
	};

}

#endif