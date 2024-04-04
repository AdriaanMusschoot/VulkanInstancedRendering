#ifndef VULKAN_ENGINE_H
#define VULKAN_ENGINE_H
#include <GLFW/glfw3.h>
#include "Configuration.h"

class VulkanEngine final
{
public:
	VulkanEngine();
	~VulkanEngine();

	VulkanEngine(const VulkanEngine& other) = delete;
	VulkanEngine(VulkanEngine&& other) = delete;
	VulkanEngine& operator=(const VulkanEngine& other) = delete;
	VulkanEngine& operator=(VulkanEngine&& other) = delete;

private:
	bool m_IsDebugging{ true };

	const std::string m_WindowName{ "GP2 Assignment" };
	const int m_Width{ 690 };
	const int m_Height{ 480 };
	
	//cannot have unique ptr because incomplete typedefined struct
	GLFWwindow * m_WindowPtr{ nullptr };

	vk::Instance m_Instance{ nullptr };
	vk::DebugUtilsMessengerEXT m_DebugMessenger{ nullptr };
	vk::DispatchLoaderDynamic m_DLDInstance;

	vk::PhysicalDevice m_PhysicalDevice{ nullptr };

	void CreateGLFWWindow();
	void CreateInstance();
	void CreateDevice();
};

#endif