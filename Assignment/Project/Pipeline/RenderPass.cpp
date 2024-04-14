#include "RenderPass.h"
vkInit::RenderPass::RenderPass(const RenderPassInBundle& in, bool isDebugging)
	: m_Device{ in.Device }
{
	m_RenderPass = CreateRenderPass(in.SwapchainImageFormat, in.DepthFormat, isDebugging);
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

	std::vector<vk::ClearValue> clearValueVec;

	vk::ClearValue clearColor{ std::array<float, 4>{0.f, 0.f, 0.f, 1.0f } };
	clearValueVec.emplace_back(clearColor);

	vk::ClearValue clearDepth{};
	clearDepth.depthStencil = vk::ClearDepthStencilValue{ 1.0f, 1 };
	clearValueVec.emplace_back(clearDepth);

	renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValueVec.size());
	renderPassBeginInfo.pClearValues = clearValueVec.data();

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

vk::RenderPass vkInit::RenderPass::CreateRenderPass(const vk::Format& swapchainImgFormat, const vk::Format& depthFormat,  bool isDebugging)
{
	if (isDebugging)
	{
		std::cout << "RenderPass creation started\n";
	}

	std::vector<vk::AttachmentDescription> attachmentDescriptionVec;
	std::vector<vk::AttachmentReference> attachmentReferenceVec;

	//color
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

	attachmentDescriptionVec.emplace_back(colorAttachmentDescription);

	vk::AttachmentReference colorAttachmentReference{};
	colorAttachmentReference.attachment = 0;
	colorAttachmentReference.layout = vk::ImageLayout::eColorAttachmentOptimal;

	attachmentReferenceVec.emplace_back(colorAttachmentReference);

	//depth
	vk::AttachmentDescription depthAttachmentDescription{};
	depthAttachmentDescription.flags = vk::AttachmentDescriptionFlags{};
	depthAttachmentDescription.format = depthFormat;
	depthAttachmentDescription.samples = vk::SampleCountFlagBits::e1;
	depthAttachmentDescription.loadOp = vk::AttachmentLoadOp::eClear;
	depthAttachmentDescription.storeOp = vk::AttachmentStoreOp::eStore;
	depthAttachmentDescription.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
	depthAttachmentDescription.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
	depthAttachmentDescription.initialLayout = vk::ImageLayout::eUndefined;
	depthAttachmentDescription.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

	attachmentDescriptionVec.emplace_back(depthAttachmentDescription);

	vk::AttachmentReference depthAttachmentReference{};
	depthAttachmentReference.attachment = 1;
	depthAttachmentReference.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

	attachmentReferenceVec.emplace_back(depthAttachmentReference);

	vk::SubpassDescription subpass{};
	subpass.flags = vk::SubpassDescriptionFlags{};
	subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &*std::find_if(attachmentReferenceVec.begin(), attachmentReferenceVec.end(),
		[&](const vk::AttachmentReference& attachRef)
		{
			return attachRef.layout == vk::ImageLayout::eColorAttachmentOptimal;
		});
	
	subpass.pDepthStencilAttachment = &*std::find_if(attachmentReferenceVec.begin(), attachmentReferenceVec.end(),
		[&](const vk::AttachmentReference& attachRef)
		{
			return attachRef.layout == vk::ImageLayout::eDepthStencilAttachmentOptimal;
		});;

	vk::RenderPassCreateInfo renderPassCreateInfo{};
	renderPassCreateInfo.flags = vk::RenderPassCreateFlags{};
	renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptionVec.size());
	renderPassCreateInfo.pAttachments = attachmentDescriptionVec.data();
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