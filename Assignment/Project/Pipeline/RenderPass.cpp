#include "RenderPass.h"
vkInit::RenderPass::RenderPass(const vk::Device& device, const vk::Format& swapchainImgFormat, bool isDebugging)
	: m_Device{ device }
{
	m_RenderPass = CreateRenderPass(swapchainImgFormat, isDebugging);
}

vkInit::RenderPass::~RenderPass()
{
	m_Device.destroyRenderPass(m_RenderPass);
}

void vkInit::RenderPass::BeginRenderPass(const vk::CommandBuffer& commandBuffer, const vk::Framebuffer& frameBuffer, const vk::Extent2D& swapchainExtent)
{
	vk::RenderPassBeginInfo renderPassBeginInfo{};
	renderPassBeginInfo.renderPass = m_RenderPass;
	renderPassBeginInfo.framebuffer = frameBuffer;
	renderPassBeginInfo.renderArea.offset.x = 0;
	renderPassBeginInfo.renderArea.offset.y = 0;
	renderPassBeginInfo.renderArea.extent = swapchainExtent;

	vk::ClearValue clearValue{ std::array<float, 4>{0.f, 0.f, 0.f, 1.0f } };
	renderPassBeginInfo.clearValueCount = 1;
	renderPassBeginInfo.pClearValues = &clearValue;

	commandBuffer.beginRenderPass(&renderPassBeginInfo, vk::SubpassContents::eInline);
}

void vkInit::RenderPass::EndRenderPass(const vk::CommandBuffer& commandBuffer)
{
	commandBuffer.endRenderPass();
}

vk::RenderPass const& vkInit::RenderPass::GetRenderPass() const
{
	return m_RenderPass;
}

vk::RenderPass vkInit::RenderPass::CreateRenderPass(const vk::Format& swapchainImgFormat, bool isDebugging)
{
	if (isDebugging)
	{
		std::cout << "RenderPass creation started\n";
	}

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
		return m_Device.createRenderPass(renderPassCreateInfo);
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