#include "RenderPass.h"
vkInit::RenderPass::RenderPass(const RenderPassInBundle& in)
	: m_Device{ in.Device }
{
	m_RenderPass = CreateRenderPass(in);
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

	vk::ClearValue clearColor{ vk::ClearColorValue{ 0.f, 0.f, 0.5f, 1.0f } };
	clearValueVec.emplace_back(clearColor);

	vk::ClearValue clearDepth{};
	clearDepth.depthStencil = vk::ClearDepthStencilValue{ 1.0f, 0 };
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

vk::RenderPass vkInit::RenderPass::CreateRenderPass(const RenderPassInBundle& in)
{
	std::cout << "RenderPass creation started\n";

	std::vector<vk::AttachmentDescription> attachmentDescriptionVec;
	std::vector<vk::AttachmentReference> attachmentReferenceVec;

	vk::SubpassDescription subpass{};
	subpass.flags = vk::SubpassDescriptionFlags{};
	subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;

	if ((in.AttachmentFlags | vkUtil::AttachmentFlags::Color) == in.AttachmentFlags)
	{
		vk::AttachmentDescription colorAttachmentDescription{};
		colorAttachmentDescription.flags = vk::AttachmentDescriptionFlags{};
		colorAttachmentDescription.format = in.SwapchainImageFormat;
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

	}

	if ((in.AttachmentFlags | vkUtil::AttachmentFlags::Depth) == in.AttachmentFlags)
	{
		vk::AttachmentDescription depthAttachmentDescription{};
		depthAttachmentDescription.flags = vk::AttachmentDescriptionFlags{};
		depthAttachmentDescription.format = in.DepthFormat;
		depthAttachmentDescription.samples = vk::SampleCountFlagBits::e1;
		depthAttachmentDescription.loadOp = vk::AttachmentLoadOp::eClear;
		depthAttachmentDescription.storeOp = vk::AttachmentStoreOp::eDontCare;
		depthAttachmentDescription.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
		depthAttachmentDescription.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		depthAttachmentDescription.initialLayout = vk::ImageLayout::eUndefined;
		depthAttachmentDescription.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

		attachmentDescriptionVec.emplace_back(depthAttachmentDescription);
		vk::AttachmentReference depthAttachmentReference{};
		depthAttachmentReference.attachment = 1;
		depthAttachmentReference.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

		attachmentReferenceVec.emplace_back(depthAttachmentReference);
	}
;
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

	vk::SubpassDependency dependencyInfo{};
	dependencyInfo.srcSubpass = VK_SUBPASS_EXTERNAL;
	//dependencyInfo.srcAccessMask = vk::AccessFlags{};
	dependencyInfo.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
	dependencyInfo.dstSubpass = 0;
	dependencyInfo.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
	dependencyInfo.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite;

	vk::RenderPassCreateInfo renderPassCreateInfo{};
	renderPassCreateInfo.flags = vk::RenderPassCreateFlags{};
	renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptionVec.size());
	renderPassCreateInfo.pAttachments = attachmentDescriptionVec.data();
	renderPassCreateInfo.subpassCount = 1;
	renderPassCreateInfo.pSubpasses = &subpass;
	renderPassCreateInfo.dependencyCount = 1;
	renderPassCreateInfo.pDependencies = &dependencyInfo;

	try
	{
		return m_Device.createRenderPass(renderPassCreateInfo);
	}
	catch (const vk::SystemError& systemError)
	{
		std::cout << systemError.what() << "\n";
		return nullptr;
	}
}