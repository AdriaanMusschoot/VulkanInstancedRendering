#include "VulkanEngine.h"
#include "Device/Instance.h"
#include "Utils/Logging.h"
#include "Device/Device.h"
#include "Rendering/Swapchain.h"
#include "Pipeline/Pipeline.h"
#include "Rendering/FrameBuffer.h"
#include "Rendering/Commands.h"
#include "Rendering/Synchronization.h"
#include "Utils/RenderStructs.h"
#include "Pipeline/Descriptor.h"

ave::VulkanEngine::VulkanEngine(const std::string& windowName, int width, int height, GLFWwindow* windowPtr, bool isDebugging)
	: m_WindowName{ windowName }
	, m_Width{ width }
	, m_Height{ height }
	, m_WindowPtr{ windowPtr }
	, m_IsDebugging{ isDebugging }
{
	if (m_IsDebugging)
	{
		std::cout << "Ladies and gentleman, start your engines\n";
	}

	CreateInstance();
	CreateDevice();
	CreateDescriptorSetLayout();
	CreatePipeline();
	SetUpRendering();	
	SetUpScene();
}

ave::VulkanEngine::~VulkanEngine()
{
	m_Device.waitIdle();	

	m_SceneUPtr.reset();

	m_Device.destroyCommandPool(m_CommandPool);

	m_Device.destroyPipeline(m_Pipeline);
	m_Device.destroyPipelineLayout(m_PipelineLayout);
	m_Device.destroyRenderPass(m_RenderPass);
	m_Device.destroyDescriptorSetLayout(m_DescriptorSetLayout);

	DestroySwapchain();
	m_Device.destroy();

	m_Instance.destroySurfaceKHR(m_Surface);
	if (m_IsDebugging)
	{
		m_Instance.destroyDebugUtilsMessengerEXT(m_DebugMessenger, nullptr, m_DLDInstance);
	}
	m_Instance.destroy();

	if (m_IsDebugging)
	{
		std::cout << "The engine died out\n";
	}
}

void ave::VulkanEngine::Render() 
{
	vk::Result result;

	//uint64_max is a macro to wait for ever
	result = m_Device.waitForFences(1, &m_SwapchainFrameVec[m_CurrentFrameNr].InFlightFence, VK_TRUE, UINT64_MAX);
	if (result != vk::Result::eSuccess)
	{
		if (m_IsDebugging)
		{
			std::cout << "Waiting for fence failure\n";
		}
	}

	result = m_Device.resetFences(1, &m_SwapchainFrameVec[m_CurrentFrameNr].InFlightFence);
	if (result != vk::Result::eSuccess)
	{
		if (m_IsDebugging)
		{
			std::cout << "Waiting for fence failure\n";
		}
	}
	
	uint32_t imageIndex{ m_Device.acquireNextImageKHR(m_Swapchain, UINT64_MAX, m_SwapchainFrameVec[m_CurrentFrameNr].SemaphoreImageAvailable, nullptr).value };

	vk::CommandBuffer commandBuffer{ m_SwapchainFrameVec[m_CurrentFrameNr].CommandBuffer };

	commandBuffer.reset();

	PrepareFrame(imageIndex);

	RecordDrawCommands(commandBuffer, imageIndex);

	std::vector<vk::Semaphore> waitSemaphoreVec;
	waitSemaphoreVec.emplace_back(m_SwapchainFrameVec[m_CurrentFrameNr].SemaphoreImageAvailable);

	std::vector<vk::PipelineStageFlags> waitStageVec;
	waitStageVec.emplace_back(vk::PipelineStageFlagBits::eColorAttachmentOutput);

	std::vector<vk::Semaphore> signalSemaphoreVec;
	signalSemaphoreVec.emplace_back(m_SwapchainFrameVec[m_CurrentFrameNr].SemaphoreRenderingFinished);

	vk::SubmitInfo submitInfo{};
	submitInfo.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphoreVec.size());
	submitInfo.pWaitSemaphores = waitSemaphoreVec.data();
	submitInfo.pWaitDstStageMask = waitStageVec.data();
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;
	submitInfo.signalSemaphoreCount = static_cast<uint32_t>(signalSemaphoreVec.size());
	submitInfo.pSignalSemaphores = signalSemaphoreVec.data();

	try
	{
		m_GraphicsQueue.submit(submitInfo, m_SwapchainFrameVec[m_CurrentFrameNr].InFlightFence);
	}
	catch (const vk::SystemError& systemError)
	{
		if (m_IsDebugging)
		{
			std::cout << systemError.what() << "\n";
		}
	}

	std::vector<vk::SwapchainKHR> swapchainVec;
	swapchainVec.emplace_back(m_Swapchain);

	vk::PresentInfoKHR presentInfo{};
	presentInfo.waitSemaphoreCount = static_cast<uint32_t>(signalSemaphoreVec.size());
	presentInfo.pWaitSemaphores = signalSemaphoreVec.data();
	presentInfo.swapchainCount = static_cast<uint32_t>(swapchainVec.size());
	presentInfo.pSwapchains = swapchainVec.data();
	presentInfo.pImageIndices = &imageIndex;
	
	try
	{
		result = m_PresentQueue.presentKHR(presentInfo);
	}
	catch (const vk::OutOfDateKHRError& outOfDateError)
	{
		if (m_IsDebugging)
		{
			std::cout << "Swapchain recreation\n";
			std::cout << outOfDateError.what() << "\n";
		}
		RecreateSwapchain();
		return;
	}

	m_CurrentFrameNr = (m_CurrentFrameNr + 1) % m_MaxNrFramesInFlight;
}

void ave::VulkanEngine::CreateInstance()
{
	m_Instance = vkInit::CreateInstance(m_IsDebugging, m_WindowName);

	m_DLDInstance = vk::DispatchLoaderDynamic{ m_Instance, vkGetInstanceProcAddr };

	if (m_IsDebugging)
	{
		m_DebugMessenger = vkInit::CreateDebugMessenger(m_Instance, m_DLDInstance);
	}

	VkSurfaceKHR oldStyleSurface;
	if (glfwCreateWindowSurface(m_Instance, m_WindowPtr, nullptr, &oldStyleSurface) != VK_SUCCESS)
	{
		if (m_IsDebugging)
		{
			std::cout << "Window surface creation failure\n";
		}
	}
	else if (m_IsDebugging)
	{
		std::cout << "Window surface creation successful\n";
	}
	//copy constructor that takes old surface for the new surface
	m_Surface = oldStyleSurface;
}

void ave::VulkanEngine::CreateDevice()
{
	m_PhysicalDevice = vkInit::ChoosePhysicalDevice(m_Instance, m_IsDebugging);

	m_Device = vkInit::CreateLogicalDevice(m_PhysicalDevice, m_Surface, m_IsDebugging);

	std::array<vk::Queue, 2> queues = vkInit::GetQueuesFromGPU(m_PhysicalDevice, m_Device, m_Surface, m_IsDebugging);
	m_GraphicsQueue = queues[0];
	m_PresentQueue = queues[1];

	CreateSwapchain();

	m_CurrentFrameNr = 0;
}

void ave::VulkanEngine::CreateSwapchain()
{
	vkInit::SwapchainBundle tempBunlde = vkInit::CreateSwapchain(m_PhysicalDevice, m_Device, m_Surface, m_Width, m_Height, m_IsDebugging);
	m_Swapchain = tempBunlde.Swapchain;
	m_SwapchainFrameVec = tempBunlde.FrameVec;
	m_SwapchainExtent = tempBunlde.Extent;
	m_SwapchainFormat = tempBunlde.Format;

	m_MaxNrFramesInFlight = static_cast<int>(m_SwapchainFrameVec.size());
}

void ave::VulkanEngine::CreatePipeline()
{
	vkInit::GraphicsPipelineInBundle specification{};
	specification.Device = m_Device;
	specification.VertexFilePath = "shaders/shader.vert.spv";
	specification.FragmentFilePath = "shaders/shader.frag.spv";
	specification.SwapchainExtent = m_SwapchainExtent;
	specification.SwapchainImgFormat = m_SwapchainFormat;
	specification.DescriptorSetLayout = m_DescriptorSetLayout;

	vkInit::GraphicsPipelineOutBundle out{ vkInit::CreateGraphicsPipeline(specification, m_IsDebugging) };
	m_PipelineLayout = out.Layout;
	m_RenderPass = out.RenderPass;
	m_Pipeline = out.Pipeline;
}

void ave::VulkanEngine::SetUpRendering()
{
	CreateFrameBuffers();

	m_CommandPool = vkInit::CreateCommandPool(m_Device, m_PhysicalDevice, m_Surface, m_IsDebugging);

	vkInit::CommandBufferInBundle commandBufferIn
	{
		m_Device,
		m_CommandPool,
		m_SwapchainFrameVec
	};

	m_MainCommandBuffer = vkInit::CreateMainCommandBuffer(commandBufferIn, m_IsDebugging);

	vkInit::CreateFrameCommandBuffers(commandBufferIn, m_IsDebugging);

	CreateFrameResources();
}

void ave::VulkanEngine::SetUpScene()
{
	m_SceneUPtr = std::make_unique<ave::Scene>();

	ave::MeshInBundle meshInput
	{
		m_GraphicsQueue,
		m_MainCommandBuffer
	};

	std::unique_ptr triangleMeshUPtr = std::make_unique<ave::Mesh>(m_Device, m_PhysicalDevice);
	triangleMeshUPtr->AddVertex(vkUtil::Vertex2D{ { 0.0f, -0.5f }, { 1.0f, 0.0f, 0.0f } });
	triangleMeshUPtr->AddVertex(vkUtil::Vertex2D{ { 0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f } });
	triangleMeshUPtr->AddVertex(vkUtil::Vertex2D{ { -0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f } });
		
	triangleMeshUPtr->InitializeVertexBuffer(meshInput);

	triangleMeshUPtr->AddIndex(0);
	triangleMeshUPtr->AddIndex(1);
	triangleMeshUPtr->AddIndex(2);

	triangleMeshUPtr->InitializeIndexBuffer(meshInput);

	m_SceneUPtr->AddMesh(std::move(triangleMeshUPtr));
}

void ave::VulkanEngine::PrepareFrame(uint32_t imgIdx)
{
	glm::vec3 eye = { 0.0f, 0.0f, -1.0f };
	glm::vec3 center = { 0.0f, 0.0f, 0.0f };
	glm::vec3 up = { 0.0f, 0.0f, -1.0f };
	glm::mat4 view = glm::lookAt(eye, center, up);

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), static_cast<float>(m_SwapchainExtent.width) / static_cast<float>(m_SwapchainExtent.height), 0.1f, 10.0f);
	projection[1][1] *= -1;

	
	m_SwapchainFrameVec[imgIdx].WVPMatrix.ModelMatrix = glm::mat4(1.0f);
	m_SwapchainFrameVec[imgIdx].WVPMatrix.ViewMatrix = glm::mat4(1.0f);
	m_SwapchainFrameVec[imgIdx].WVPMatrix.ProjectionMatrix = projection;
	memcpy(m_SwapchainFrameVec[imgIdx].WVPWriteLocationPtr, &(m_SwapchainFrameVec[imgIdx].WVPMatrix), sizeof(vkUtil::UBO));

	m_SwapchainFrameVec[imgIdx].WriteDescriptorSet(m_Device);
}

void ave::VulkanEngine::CreateFrameBuffers()
{
	vkInit::FrameBufferInBundle frameBufferIn;
	frameBufferIn.Device = m_Device;
	frameBufferIn.RenderPass = m_RenderPass;
	frameBufferIn.SwapchainExtent = m_SwapchainExtent;

	vkInit::CreateFrameBuffers(frameBufferIn, m_SwapchainFrameVec, m_IsDebugging);
}

void ave::VulkanEngine::CreateFrameResources()
{
	vkInit::DescriptorSetLayoutData setLayoutData;
	setLayoutData.Count = 1;
	setLayoutData.TypeVec.emplace_back(vk::DescriptorType::eUniformBuffer);
	m_DescriptorPool = vkInit::CreateDescriptorPool(m_Device, static_cast<uint32_t>(m_SwapchainFrameVec.size()), setLayoutData, m_IsDebugging);

	for (auto& frame : m_SwapchainFrameVec)
	{
		frame.InFlightFence = vkInit::CreateFence(m_Device, m_IsDebugging);
		frame.SemaphoreImageAvailable = vkInit::CreateSemaphore(m_Device, m_IsDebugging);
		frame.SemaphoreRenderingFinished = vkInit::CreateSemaphore(m_Device, m_IsDebugging);

		frame.CreateUBOResources(m_Device, m_PhysicalDevice);
		frame.UBODescriptorSet = vkInit::CreateDescriptorSet(m_Device, m_DescriptorPool, m_DescriptorSetLayout, m_IsDebugging);
	}
}

void ave::VulkanEngine::CreateDescriptorSetLayout()
{
	vkInit::DescriptorSetLayoutData setLayoutData;
	setLayoutData.Count = 1;
	setLayoutData.IndexVec.emplace_back(0);
	setLayoutData.TypeVec.emplace_back(vk::DescriptorType::eUniformBuffer);
	setLayoutData.CountVec.emplace_back(1);
	setLayoutData.StageFlagVec.emplace_back(vk::ShaderStageFlagBits::eVertex);

	m_DescriptorSetLayout = vkInit::CreateDescriptorSetLayout(m_Device, setLayoutData, m_IsDebugging);
}

void ave::VulkanEngine::RecordDrawCommands(const vk::CommandBuffer& commandBuffer, uint32_t imageIndex)
{
	vk::CommandBufferBeginInfo bufferBeginInfo{};
	try
	{
		commandBuffer.begin(bufferBeginInfo);
	}
	catch (const vk::SystemError& systemError)
	{
		if (m_IsDebugging)
		{
			std::cout << systemError.what() << "\n";
		}
	}

	vk::RenderPassBeginInfo renderPassBeginInfo{};
	renderPassBeginInfo.renderPass = m_RenderPass;
	renderPassBeginInfo.framebuffer = m_SwapchainFrameVec[imageIndex].Framebuffer;
	renderPassBeginInfo.renderArea.offset.x = 0;
	renderPassBeginInfo.renderArea.offset.y = 0;
	renderPassBeginInfo.renderArea.extent = m_SwapchainExtent;

	vk::ClearValue clearValue{ std::array<float, 4>{1.0f, 0.5f, 0.25f, 1.0f } };
	renderPassBeginInfo.clearValueCount = 1;
	renderPassBeginInfo.pClearValues = &clearValue;

	commandBuffer.beginRenderPass(&renderPassBeginInfo, vk::SubpassContents::eInline);

	commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_PipelineLayout, 0, m_SwapchainFrameVec[imageIndex].UBODescriptorSet, nullptr);

	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_Pipeline);

	m_SceneUPtr->Draw(commandBuffer);

	commandBuffer.endRenderPass();

	try
	{
		commandBuffer.end();
	}
	catch (const vk::SystemError& systemError)
	{
		if (m_IsDebugging)
		{
			std::cout << systemError.what() << "\n";
		}
	}
}

void ave::VulkanEngine::RecreateSwapchain()
{
	m_Width = 0;
	m_Height = 0;
	while (m_Width == 0 or m_Height == 0)
	{
		glfwGetFramebufferSize(m_WindowPtr, &m_Width, &m_Height);
		glfwWaitEvents();
	}

	m_Device.waitIdle();

	DestroySwapchain();
	CreateSwapchain();
	CreateFrameBuffers();
	CreateFrameResources();

	vkInit::CommandBufferInBundle commandBufferIn
	{
		m_Device,
		m_CommandPool,
		m_SwapchainFrameVec
	};
	vkInit::CreateFrameCommandBuffers(commandBufferIn, m_IsDebugging);
}

void ave::VulkanEngine::DestroySwapchain()
{
	for (auto& frame : m_SwapchainFrameVec)
	{
		m_Device.destroySemaphore(frame.SemaphoreRenderingFinished);
		m_Device.destroySemaphore(frame.SemaphoreImageAvailable);
		m_Device.destroyFence(frame.InFlightFence);
		m_Device.destroyFramebuffer(frame.Framebuffer);
		m_Device.destroyImageView(frame.ImageView);
		m_Device.unmapMemory(frame.WVPBuffer.BufferMemory);
		m_Device.freeMemory(frame.WVPBuffer.BufferMemory);
		m_Device.destroyBuffer(frame.WVPBuffer.Buffer);
	}
	m_Device.destroyDescriptorPool(m_DescriptorPool);	

	m_Device.destroySwapchainKHR(m_Swapchain);
}
