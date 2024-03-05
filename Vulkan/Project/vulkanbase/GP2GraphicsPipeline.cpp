#include "GP2GraphicsPipeline.h"

//void GraphicsPipeline::BeginRenderPass(const VkCommandBuffer& commandBuffer)
//{
//	VkRenderPassBeginInfo renderPassInfo{};
//	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
//	renderPassInfo.renderPass = renderPass;
//	renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
//	renderPassInfo.renderArea.offset = { 0, 0 };
//	renderPassInfo.renderArea.extent = swapChainExtent;
//
//	VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
//	renderPassInfo.clearValueCount = 1;
//	renderPassInfo.pClearValues = &clearColor;
//
//	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
//}
//
//void GraphicsPipeline::SetViewPort(const VkCommandBuffer& commandBuffer)
//{
//	VkViewport viewport{};
//	viewport.x = 0.0f;
//	viewport.y = 0.0f;
//	viewport.width = (float)swapChainExtent.width;
//	viewport.height = (float)swapChainExtent.height;
//	viewport.minDepth = 0.0f;
//	viewport.maxDepth = 1.0f;
//	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
//}
//
//void GraphicsPipeline::SetScissor(const VkCommandBuffer& commandBuffer)
//{
//	VkRect2D scissor{};
//	scissor.offset = { 0, 0 };
//	scissor.extent = swapChainExtent;
//	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
//}
//
//void GraphicsPipeline::EndRenderPass(const VkCommandBuffer& commandBuffer)
//{
//	vkCmdEndRenderPass(commandBuffer);
//}
