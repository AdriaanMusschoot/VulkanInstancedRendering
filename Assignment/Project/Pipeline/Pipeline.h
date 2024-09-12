#ifndef VK_PIPELINE_H
#define VK_PIPELINE_H
#include "Engine/Configuration.h"
#include "Shader.h"
#include "Utils/RenderStructs.h"
#include "RenderPass.h"
#include "functional"

namespace vkInit
{

	template <vkUtil::Vertex VertexStruct>
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
			std::vector<vk::DescriptorSetLayout> DescriptorSetLayoutVec;
		};

		struct GraphicsPipelineOutBundle
		{
			vk::PipelineLayout Layout;
			vk::Pipeline Pipeline;
		};

		Pipeline(GraphicsPipelineInBundle const& in)
			: m_Device{ in.Device }
		{
			GraphicsPipelineOutBundle out = CreateGraphicsPipeline(in);
			m_PipelineLayout = out.Layout;
			m_Pipeline = out.Pipeline;
		}
		~Pipeline()
		{
			m_Device.destroyPipeline(m_Pipeline);
			m_Device.destroyPipelineLayout(m_PipelineLayout);
		}

		Pipeline(Pipeline const& other) = delete;
		Pipeline(Pipeline&& other) = delete;
		Pipeline& operator=(Pipeline const& other) = delete;
		Pipeline& operator=(Pipeline&& other) = delete;

		void Record(vk::CommandBuffer const& commandBuffer, vk::Framebuffer const& frameBuffer, vk::Extent2D const& swapchainExtent, vk::DescriptorSet const& descriptorSet)
		{
			commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_PipelineLayout, 0, descriptorSet, nullptr);

			commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_Pipeline);
		}
		
		vk::PipelineLayout const& GetPipelineLayout() const
		{
			return m_PipelineLayout;
		}
	private:
		vk::PipelineLayout m_PipelineLayout;
		vk::Pipeline m_Pipeline;

		vk::Device m_Device;

		vk::PipelineLayout CreatePipelineLayout(vk::Device const& device, std::vector<vk::DescriptorSetLayout> const& descriptorSetLayout)
		{
			vk::PushConstantRange pushConstantRange{};
			pushConstantRange.offset = 0;
			pushConstantRange.size = sizeof(glm::mat4);
			pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eVertex;

			vk::PipelineLayoutCreateInfo layoutCreateInfo{};
			layoutCreateInfo.flags = vk::PipelineLayoutCreateFlags{};
			layoutCreateInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayout.size());
			layoutCreateInfo.pSetLayouts = descriptorSetLayout.data();
			layoutCreateInfo.pushConstantRangeCount = 1;
			layoutCreateInfo.pPushConstantRanges = &pushConstantRange;

			try
			{
				return device.createPipelineLayout(layoutCreateInfo);
			}
			catch (const vk::SystemError& systemError)
			{
				std::cout << systemError.what() << "\n";

				return nullptr;
			}
		}
		vk::PipelineVertexInputStateCreateInfo PopulateVertexInput(std::vector<vk::VertexInputBindingDescription> const& bindingDescriptionVec, std::vector<vk::VertexInputAttributeDescription> const& attributeDescriptionVec)
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
		vk::PipelineShaderStageCreateInfo PopulateShaderStage(vk::ShaderModule const& shaderModule, vk::ShaderStageFlagBits const& flagBits)
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
		vk::PipelineViewportStateCreateInfo PopulateViewportState(vk::Viewport const& viewport, vk::Rect2D const& scissor)
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
			rasterizerStateCreateInfo.cullMode = vk::CullModeFlagBits::eNone;
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
		vk::PipelineColorBlendStateCreateInfo PopulateColorBlendState(vk::PipelineColorBlendAttachmentState const& colorBlendAttachment)
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
		GraphicsPipelineOutBundle CreateGraphicsPipeline(GraphicsPipelineInBundle const& in)
		{
			std::cout << "\nPipeline creation started\n";

			vk::GraphicsPipelineCreateInfo pipelineCreateInfo{};
			pipelineCreateInfo.flags = vk::PipelineCreateFlags{};

			std::vector<vk::PipelineShaderStageCreateInfo> shaderStageCreateInfoVec{};

			//Vertex input/what we will be sending
			std::vector<vk::VertexInputBindingDescription> bindingDescription{ VertexStruct::GetBindingDescription() };
			std::vector attributeDescriptionArr{ VertexStruct::GetAttributeDescription() };

			vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{ PopulateVertexInput(bindingDescription, attributeDescriptionArr) };
			pipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;

			//Input assembly/how should thy interpreteth it
			vk::PipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo{ PopulateInputAssembly() };
			pipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;

			std::cout << "\tShader module creation started\n";

			vk::ShaderModule vertexShaderModule{ vkUtil::CreateModule(in.Device, in.VertexFilePath) };
			vk::PipelineShaderStageCreateInfo vertexShaderStageCreateInfo{ PopulateShaderStage(vertexShaderModule, vk::ShaderStageFlagBits::eVertex) };

			vk::ShaderModule fragmentShaderModule{ vkUtil::CreateModule(in.Device, in.FragmentFilePath) };
			vk::PipelineShaderStageCreateInfo fragmentShaderStageCreateInfo{ PopulateShaderStage(fragmentShaderModule, vk::ShaderStageFlagBits::eFragment) };

			shaderStageCreateInfoVec.emplace_back(vertexShaderStageCreateInfo);
			shaderStageCreateInfoVec.emplace_back(fragmentShaderStageCreateInfo);

			pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStageCreateInfoVec.size());
			pipelineCreateInfo.pStages = shaderStageCreateInfoVec.data();

			std::cout << "\tDepth creation started\n";

			vk::PipelineDepthStencilStateCreateInfo depthStateCreateInfo{ PopulateDepthState() };
			pipelineCreateInfo.pDepthStencilState = &depthStateCreateInfo;

			std::cout << "\tViewport creation started\n";

			vk::Viewport viewport{};
			viewport.x = 0;
			viewport.y = 0;
			viewport.width = static_cast<float>(in.SwapchainExtent.width);
			viewport.height = static_cast<float>(in.SwapchainExtent.height);
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.f;

			std::cout << "\tScissor creation started\n";

			vk::Rect2D scissor{};
			scissor.offset.x = 0;
			scissor.offset.y = 0;
			scissor.extent = in.SwapchainExtent;

			vk::PipelineViewportStateCreateInfo viewportStateCreateInfo{ PopulateViewportState(viewport, scissor) };
			pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;

			std::cout << "\tRasterizer creation started\n";

			vk::PipelineRasterizationStateCreateInfo rasterizerStateCreateInfo{ PopulateRasterizationState() };
			pipelineCreateInfo.pRasterizationState = &rasterizerStateCreateInfo;

			std::cout << "\tMultisample creation started\n";

			vk::PipelineMultisampleStateCreateInfo multisampleStateCreateInfo{ PopulateMultisampleState() };
			pipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;

			std::cout << "\tColor blend creation started\n";

			vk::PipelineColorBlendAttachmentState colorBlendAttachmentState{ PopulateColorBlendAttachmentState() };
			vk::PipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{ PopulateColorBlendState(colorBlendAttachmentState) };
			pipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;

			std::cout << "\tPipeline layout creation started\n";

			vk::PipelineLayout pipelineLayout{ CreatePipelineLayout(in.Device, in.DescriptorSetLayoutVec) };
			pipelineCreateInfo.layout = pipelineLayout;

			std::cout << "\tRenderpass creation started\n";

			pipelineCreateInfo.renderPass = in.RenderPass;

			pipelineCreateInfo.basePipelineHandle = nullptr;

			std::cout << "Pipeline creation ended\n";

			vk::Pipeline pipeline{};

			try
			{
				pipeline = in.Device.createGraphicsPipeline(nullptr, pipelineCreateInfo).value;
			}
			catch (const vk::SystemError& systemError)
			{
				std::cout << systemError.what() << "\n";
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