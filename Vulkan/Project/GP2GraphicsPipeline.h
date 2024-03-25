#pragma once
#include "vulkanbase/VulkanUtil.h"
#include "GP2Shader.h"

namespace amu
{
	template<VertexConcept VS>
	class GraphicsPipeline
	{
	public:
		void CreateGraphicsPipeline(VkDevice device, Shader<VS>& shader, VkRenderPass renderPass)
		{
			m_VkDevice = device;

			VkPipelineViewportStateCreateInfo viewportState{};
			viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			viewportState.viewportCount = 1;
			viewportState.scissorCount = 1;

			VkPipelineRasterizationStateCreateInfo rasterizer{};
			rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			rasterizer.depthClampEnable = VK_FALSE;
			rasterizer.rasterizerDiscardEnable = VK_FALSE;
			rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
			rasterizer.lineWidth = 1.0f;
			rasterizer.cullMode = VK_CULL_MODE_NONE;
			rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
			rasterizer.depthBiasEnable = VK_FALSE;

			VkPipelineMultisampleStateCreateInfo multisampling{};
			multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			multisampling.sampleShadingEnable = VK_FALSE;
			multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

			VkPipelineColorBlendAttachmentState colorBlendAttachment{};
			colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
			colorBlendAttachment.blendEnable = VK_FALSE;

			VkPipelineColorBlendStateCreateInfo colorBlending{};
			colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			colorBlending.logicOpEnable = VK_FALSE;
			colorBlending.logicOp = VK_LOGIC_OP_COPY;
			colorBlending.attachmentCount = 1;
			colorBlending.pAttachments = &colorBlendAttachment;
			colorBlending.blendConstants[0] = 0.0f;
			colorBlending.blendConstants[1] = 0.0f;
			colorBlending.blendConstants[2] = 0.0f;
			colorBlending.blendConstants[3] = 0.0f;

			std::vector<VkDynamicState> dynamicStates = {
				VK_DYNAMIC_STATE_VIEWPORT,
				VK_DYNAMIC_STATE_SCISSOR
			};
			VkPipelineDynamicStateCreateInfo dynamicState{};
			dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
			dynamicState.pDynamicStates = dynamicStates.data();

			VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
			pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipelineLayoutInfo.setLayoutCount = 0;
			pipelineLayoutInfo.pushConstantRangeCount = 0;

			if (vkCreatePipelineLayout(m_VkDevice, &pipelineLayoutInfo, nullptr, &m_VkPipelineLayout) != VK_SUCCESS) {
				throw std::runtime_error("failed to create pipeline layout!");
			}

			VkGraphicsPipelineCreateInfo pipelineInfo{};

			pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

			pipelineInfo.stageCount = 2;
			pipelineInfo.pStages = shader.GetShaderStages().data();

			pipelineInfo.pVertexInputState = &shader.CreateVertexInputStateInfo();
			pipelineInfo.pInputAssemblyState = &shader.CreateInputAssemblyStateInfo();
#pragma region pipelineinfo
			pipelineInfo.pViewportState = &viewportState;
			pipelineInfo.pRasterizationState = &rasterizer;
			pipelineInfo.pMultisampleState = &multisampling;
			pipelineInfo.pColorBlendState = &colorBlending;
			pipelineInfo.pDynamicState = &dynamicState;
			pipelineInfo.layout = m_VkPipelineLayout;
			pipelineInfo.renderPass = renderPass;
			pipelineInfo.subpass = 0;
			pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
#pragma endregion
			if (vkCreateGraphicsPipelines(m_VkDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_VkGraphicsPipeline) != VK_SUCCESS) {
				throw std::runtime_error("failed to create graphics pipeline!");
			}

			shader.DestroyShaderModules();
		}
		void BindPipeline(const VkCommandBuffer& commandBuffer, VkExtent2D swapchainExtent)
		{
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_VkGraphicsPipeline);

			//todo the vertex buffer is now in the mesh fix it

			VkViewport viewport{};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = (float)swapchainExtent.width;
			viewport.height = (float)swapchainExtent.height;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

			VkRect2D scissor{};
			scissor.offset = { 0, 0 };
			scissor.extent = swapchainExtent;
			vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
		}
		void Destroy()
		{
			vkDestroyPipeline(m_VkDevice, m_VkGraphicsPipeline, nullptr);
			vkDestroyPipelineLayout(m_VkDevice, m_VkPipelineLayout, nullptr);
		}
		//Main thin that should be configurable is the shaders u use in the pipeline
		//You could make one massive struct render context to pass to here
	private:
		VkPipeline m_VkGraphicsPipeline;
		VkPipelineLayout m_VkPipelineLayout;

		VkDevice m_VkDevice;
	};

}