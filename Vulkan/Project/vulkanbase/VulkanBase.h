#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include "VulkanUtil.h"

#include <iostream>
#include <stdexcept>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <optional>
#include <set>
#include <limits>
#include <algorithm>
#include "GP2CommandPool.h"
#include "GP2Shader.h"
#include "GP2CommandBuffer.h"
#include "GP2Mesh.h"


const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

class VulkanBase {
public:
	void run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

private:
	void initVulkan() {
		// week 06
		createInstance();
		setupDebugMessenger();
		createSurface();

		// week 05
		pickPhysicalDevice();
		createLogicalDevice();

		// week 04 
		createSwapChain();
		createImageViews();

		// week 03
		m_GradientShader.Initialize(device);
		createRenderPass();
		createGraphicsPipeline();
		createFrameBuffers();
		// week 02
		m_CommandPool.CreateCommandPool(device, findQueueFamilies(physicalDevice));
		m_CommandBuffer.SetCommandBuffer(m_CommandPool.CreateCommandBuffer());

		CreateCircle();
		CreateTriangle();

		// week 06
		createSyncObjects();
	}

	void mainLoop() {
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
			// week 06
			drawFrame();
		}
		vkDeviceWaitIdle(device);
	}

	void cleanup() {

		vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
		vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
		vkDestroyFence(device, inFlightFence, nullptr);

		m_CommandPool.DestroyCommandPool();
		m_CircleMesh.Destroy();
		m_TriangleMesh.Destroy();

		for (auto framebuffer : swapChainFramebuffers) {
			vkDestroyFramebuffer(device, framebuffer, nullptr);
		}

		vkDestroyPipeline(device, graphicsPipeline, nullptr);
		vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
		vkDestroyRenderPass(device, renderPass, nullptr);

		for (auto imageView : swapChainImageViews) {
			vkDestroyImageView(device, imageView, nullptr);
		}

		if (enableValidationLayers) {
			DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		}
		vkDestroySwapchainKHR(device, swapChain, nullptr);
		vkDestroyDevice(device, nullptr);

		vkDestroySurfaceKHR(instance, surface, nullptr);
		vkDestroyInstance(instance, nullptr);

		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void CreateCircle()
	{
		constexpr double radius{ 0.7 };
		constexpr int nrOfPoints{ 20000 };
		std::vector<amu::Mesh::Vertex> temp;
		for (int idx{ 0 }; idx < nrOfPoints; ++idx)
		{
			double theta = 2.0 * 3.14 * idx / nrOfPoints;
			amu::Mesh::Vertex vert{};
			vert.pos.x = radius * std::cos(theta);
			vert.pos.y = radius * std::sin(theta);

			const float hue = static_cast<float>(idx) / static_cast<float>(nrOfPoints);
			vert.color = glm::vec3(glm::abs(glm::cos(hue * 3.14 * 2.0f)), glm::abs(glm::sin(hue * 3.14 * 2.0f)), 0.5f);
			temp.emplace_back(std::move(vert));
		}

		for (int idx{ 0 }; idx < temp.size(); ++idx)
		{
			if (idx < temp.size() - 1)
			{
				m_CircleMesh.AddVertex(std::move(temp[idx + 1]));
				m_CircleMesh.AddVertex(amu::Mesh::Vertex{ glm::vec2{ 0, 0 }, glm::vec3{ 1, 1, 1 } });
				m_CircleMesh.AddVertex(std::move(temp[idx]));
			}
			else
			{
				m_CircleMesh.AddVertex(std::move(temp[0]));
				m_CircleMesh.AddVertex(amu::Mesh::Vertex{ glm::vec2{ 0, 0 }, glm::vec3{ 1, 1, 1 } });
				m_CircleMesh.AddVertex(std::move(temp[idx]));
			}
		}

		m_CircleMesh.Initialize(physicalDevice, device);
	}

	void CreateTriangle()
	{
		m_TriangleMesh.AddVertex({ { 0.0f, -0.5f }, { 1.0f, 0.0f, 0.0f } });
		m_TriangleMesh.AddVertex({ { 0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f } });
		m_TriangleMesh.AddVertex({ { -0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f } });

		m_TriangleMesh.Initialize(physicalDevice, device);
	}

	void createSurface() {
		if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface!");
		}
	}

	amu::Shader m_GradientShader{ "shaders/shader.vert.spv", "shaders/shader.frag.spv" };
	amu::CommandPool m_CommandPool{};
	amu::CommandBuffer m_CommandBuffer{};
	amu::Mesh m_CircleMesh{};
	amu::Mesh m_TriangleMesh{};
	// Week 01: 
	// Actual window
	// simple fragment + vertex shader creation functions
	// These 5 functions should be refactored into a separate C++ class
	// with the correct internal state.

	GLFWwindow* window;
	void initWindow();

	// Week 02
	// Queue families
	// CommandBuffer concept
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

	void drawFrame(uint32_t imageIndex, const VkCommandBuffer& commandBuffer);
	
	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
	
	// Week 03
	// Renderpass concept
	// Graphics pipeline

	std::vector<VkFramebuffer> swapChainFramebuffers;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	VkRenderPass renderPass;

	void createFrameBuffers();
	void createRenderPass();
	void createGraphicsPipeline();

	// Week 04
	// Swap chain and image view support

	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	std::vector<VkImageView> swapChainImageViews;

	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	void createSwapChain();
	void createImageViews();

	// Week 05 
	// Logical and physical device

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	
	void pickPhysicalDevice();
	bool isDeviceSuitable(VkPhysicalDevice device);
	void createLogicalDevice();

	// Week 06
	// Main initialization

	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkDevice device = VK_NULL_HANDLE;
	VkSurfaceKHR surface;

	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;
	VkFence inFlightFence;

	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	void setupDebugMessenger();
	std::vector<const char*> getRequiredExtensions();
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	void createInstance();

	void createSyncObjects();
	void drawFrame();

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
		return VK_FALSE;
	}
};