#include "vulkanbase/VulkanBase.h"

void VulkanBase::initWindow() {
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(WIDTH, HEIGHT, "Not Vulkan", nullptr, nullptr);
}

void VulkanBase::drawScene() {
	vkCmdDraw(m_CommandBuffer.GetCommandBuffer(), 6, 1, 0, 0);
}