#ifndef VK_PIPELINE_H
#define VK_PIPELINE_H
#include "Engine/Configuration.h"
#include "Shader.h"
#include "Utils/RenderStructs.h"
#include "Rendering/Scene.h"
#include "functional"
namespace vkInit
{

	class Pipeline final
	{
	public:

		struct GraphicsPipelineInBundle
		{
			vk::Device Device;
			std::string	VertexFilePath;
			std::string FragmentFilePath;
			vk::Extent2D SwapchainExtent;
			vk::Format SwapchainImgFormat;
			vk::DescriptorSetLayout DescriptorSetLayout;
			std::function<std::vector<vk::VertexInputBindingDescription>()> GetBindingDescription;
			std::function<std::vector<vk::VertexInputAttributeDescription>()> GetAttributeDescription;
		};
	
		struct GraphicsPipelineOutBundle
		{
			vk::PipelineLayout Layout;
			vk::RenderPass RenderPass;
			vk::Pipeline Pipeline;
		};

		Pipeline(const GraphicsPipelineInBundle& in, bool isDebugging);
		~Pipeline();

		Pipeline(const Pipeline& other) = delete;
		Pipeline(Pipeline&& other) = delete;
		Pipeline& operator=(const Pipeline& other) = delete;
		Pipeline& operator=(Pipeline&& other) = delete;

		void Record(const vk::CommandBuffer& commandBuffer, const vk::Framebuffer& frameBuffer, const vk::Extent2D& swapchainExtent, const vk::DescriptorSet& descriptorSet);
		void SetScene(std::unique_ptr<ave::Scene> sceneUPtr);
		vk::RenderPass const& GetRenderPass() const;
	private:
		vk::PipelineLayout m_PipelineLayout;
		vk::Pipeline m_Pipeline;
		vk::RenderPass m_RenderPass;
		vk::Device m_Device;

		std::unique_ptr<ave::Scene> m_SceneUPtr;

		vk::PipelineLayout CreatePipelineLayout(const vk::Device& device, const vk::DescriptorSetLayout& descriptorSetLayout, bool isDebugging);
		vk::RenderPass CreateRenderPass(const vk::Device& device, const vk::Format& swapchainImgFormat, bool isDebugging);
		vk::PipelineVertexInputStateCreateInfo PopulateVertexInput(const std::vector<vk::VertexInputBindingDescription>& bindingDescriptionVec, const std::vector<vk::VertexInputAttributeDescription>& attributeDescriptionVec);
		vk::PipelineInputAssemblyStateCreateInfo PopulateInputAssembly();
		vk::PipelineShaderStageCreateInfo PopulateShaderStage(const vk::ShaderModule& shaderModule, const vk::ShaderStageFlagBits& flagBits);
		vk::PipelineRasterizationStateCreateInfo PopulateRasterizationState();
		vk::PipelineViewportStateCreateInfo PopulateViewportState(const vk::Viewport& viewport, const vk::Rect2D& scissor);
		vk::PipelineMultisampleStateCreateInfo PopulateMultisampleState();
		vk::PipelineColorBlendAttachmentState PopulateColorBlendAttachmentState();
		vk::PipelineColorBlendStateCreateInfo PopulateColorBlendState(const vk::PipelineColorBlendAttachmentState& colorBlendAttachment);
		GraphicsPipelineOutBundle CreateGraphicsPipeline(const GraphicsPipelineInBundle& in, bool isDebugging);
	};






}

#endif