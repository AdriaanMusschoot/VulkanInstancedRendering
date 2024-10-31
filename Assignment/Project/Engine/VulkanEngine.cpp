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
#include "Utils/Frame.h"
#include "Clock.h"
#include <algorithm>
#include <execution>

ave::VulkanEngine::VulkanEngine(const std::string& windowName, int width, int height, GLFWwindow* windowPtr)
	: m_WindowName{ windowName }
	, m_Width{ width }
	, m_Height{ height }
	, m_WindowPtr{ windowPtr }
{
	std::cout << "Ladies and gentleman, start your engines\n";

	CreateInstance();
	CreateDevice();
	CreateDescriptorSetLayouts();
	CreatePipelines();
	SetUpRendering();	
	PrintKeyBindings();
}

ave::VulkanEngine::~VulkanEngine()
{
	m_Device.waitIdle();	

	m_RenderPassUPtr.reset();
	m_Pipeline3DUPtr.reset();
	m_InstancedScene3DUPtr.reset();

	m_Device.destroyCommandPool(m_CommandPool);

	m_Device.destroyDescriptorSetLayout(m_DescriptorSetLayoutFrame);
	m_Device.destroyDescriptorSetLayout(m_DescriptorSetLayoutMesh);
	m_Device.destroyDescriptorPool(m_DescriptorPoolMesh);

	DestroySwapchain();

	m_Device.destroy();

	m_Instance.destroySurfaceKHR(m_Surface);
	
	m_Instance.destroyDebugUtilsMessengerEXT(m_DebugMessenger, nullptr, m_DLDInstance);
	
	m_Instance.destroy();

	std::cout << "The engine died out\n";
}

void ave::VulkanEngine::Render() 
{
	vk::Result result;

	//uint64_max is a macro to wait for ever
	result = m_Device.waitForFences(1, &m_SwapchainFrameVec[m_CurrentFrameNr].InFlightFence, VK_TRUE, UINT64_MAX);
	if (result != vk::Result::eSuccess)
	{
		std::cout << "Waiting for fence failure\n";
	}

	result = m_Device.resetFences(1, &m_SwapchainFrameVec[m_CurrentFrameNr].InFlightFence);
	if (result != vk::Result::eSuccess)
	{
		std::cout << "Waiting for fence failure\n";
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
		std::cout << systemError.what() << "\n";
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
		std::cout << "Swapchain recreation\n";
		std::cout << outOfDateError.what() << "\n";

		RecreateSwapchain();
		return;
	}

	m_CurrentFrameNr = (m_CurrentFrameNr + 1) % m_MaxNrFramesInFlight;
}

void ave::VulkanEngine::CreateInstance()
{
	m_Instance = vkInit::CreateInstance(m_WindowName);

	m_DLDInstance = vk::DispatchLoaderDynamic{ m_Instance, vkGetInstanceProcAddr };

	m_DebugMessenger = vkInit::CreateDebugMessenger(m_Instance, m_DLDInstance);

	VkSurfaceKHR oldStyleSurface;
	if (glfwCreateWindowSurface(m_Instance, m_WindowPtr, nullptr, &oldStyleSurface) != VK_SUCCESS)
	{
		std::cout << "Window surface creation failure\n";
	}
	else
	{
		std::cout << "Window surface creation successful\n";
	}
	//copy constructor that takes old surface for the new surface
	m_Surface = oldStyleSurface;
}

void ave::VulkanEngine::CreateDevice()
{
	m_PhysicalDevice = vkInit::ChoosePhysicalDevice(m_Instance);

	m_Device = vkInit::CreateLogicalDevice(m_PhysicalDevice, m_Surface);

	std::array<vk::Queue, 2> queues = vkInit::GetQueuesFromGPU(m_PhysicalDevice, m_Device, m_Surface);
	m_GraphicsQueue = queues[0];
	m_PresentQueue = queues[1];

	CreateSwapchain();

	m_CurrentFrameNr = 0;
}

void ave::VulkanEngine::CreateSwapchain()
{
	vkInit::SwapchainBundle tempBunlde = vkInit::CreateSwapchain(m_PhysicalDevice, m_Device, m_Surface, m_Width, m_Height);
	m_Swapchain = tempBunlde.Swapchain;
	m_SwapchainFrameVec = tempBunlde.FrameVec;
	m_SwapchainExtent = tempBunlde.Extent;
	m_SwapchainFormat = tempBunlde.Format;

	m_MaxNrFramesInFlight = static_cast<int>(m_SwapchainFrameVec.size());

	for (auto& frame : m_SwapchainFrameVec)
	{
		frame.Device = m_Device;
		frame.PhysicalDevice = m_PhysicalDevice;
		frame.DepthExtent = m_SwapchainExtent;

		frame.CreateDepthResources();
	}
}

void ave::VulkanEngine::CreatePipelines()
{
	vkInit::RenderPassInBundle inRenderPass{};
	inRenderPass.Device = m_Device;
	inRenderPass.DepthFormat = m_SwapchainFrameVec[0].DepthFormat;
	inRenderPass.SwapchainImageFormat = m_SwapchainFormat;
	inRenderPass.AttachmentFlags = static_cast<vkUtil::AttachmentFlags>(vkUtil::AttachmentFlags::Color | vkUtil::AttachmentFlags::Depth);
	m_RenderPassUPtr = std::make_unique<vkInit::RenderPass>(inRenderPass);

	vkInit::Pipeline<vkUtil::Vertex3D>::GraphicsPipelineInBundle specification3D{};
	specification3D.Device = m_Device;
	specification3D.SwapchainExtent = m_SwapchainExtent;
	specification3D.DescriptorSetLayoutVec.emplace_back(m_DescriptorSetLayoutFrame);
	specification3D.DescriptorSetLayoutVec.emplace_back(m_DescriptorSetLayoutMesh);
	specification3D.VertexFilePath = "shaders/Shader3D.vert.spv";
	specification3D.FragmentFilePath = "shaders/Shader3D.frag.spv";
	specification3D.RenderPass = m_RenderPassUPtr->GetRenderPass();

	m_Pipeline3DUPtr = std::make_unique<vkInit::Pipeline<vkUtil::Vertex3D>>(specification3D);
}

void ave::VulkanEngine::SetUpRendering()
{
	CreateFrameBuffers();

	m_CommandPool = vkInit::CreateCommandPool(m_Device, m_PhysicalDevice, m_Surface);

	vkInit::CommandBufferInBundle commandBufferIn
	{
		m_Device,
		m_CommandPool,
		m_SwapchainFrameVec
	};

	m_MainCommandBuffer = vkInit::CreateMainCommandBuffer(commandBufferIn);

	vkInit::CreateFrameCommandBuffers(commandBufferIn);

	CreateFrameResources();

	vkInit::DescriptorSetLayoutData descriptorSetLayoutData{};
	descriptorSetLayoutData.Count = 1;
	descriptorSetLayoutData.TypeVec.emplace_back(vk::DescriptorType::eCombinedImageSampler);
	//allow for 10 textures
	m_DescriptorPoolMesh = vkInit::CreateDescriptorPool(m_Device, m_NumberOfTextures, descriptorSetLayoutData);

	m_CameraUPtr = std::make_unique<Camera>(m_WindowPtr, glm::vec3{ 0, 0, -300 }, 20, m_SwapchainExtent.width, m_SwapchainExtent.height);

	Create3DScene();
}

void ave::VulkanEngine::Create3DScene()
{
	using V3D = vkUtil::Vertex3D;
	m_InstancedScene3DUPtr = std::make_unique<InstancedScene<V3D>>();

	vkUtil::MeshInBundle meshIn
	{
		m_GraphicsQueue,
		m_MainCommandBuffer,
		m_Device,
		m_PhysicalDevice
	};

	std::vector<V3D> ferrariVertexVec{};
	std::vector<uint32_t> ferrariIndexVec{};

	vkUtil::ParseOBJ<V3D>("Resources/ferrari.obj", ferrariVertexVec, ferrariIndexVec, false);

	std::vector<glm::mat4> ferrariPositionVec{};
	int numRows = 100;  
	int numCols = 100;  
	float spacingX = 30;
	float spacingY = 90;
	for (int rowIdx = 0; rowIdx < numRows; ++rowIdx) 
	{
		for (int colIdx = 0; colIdx < numCols; ++colIdx)
		{
			glm::mat4 const translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(colIdx * spacingX, 0.0f, rowIdx * spacingY));

			glm::mat4 const rotationMatrix = glm::rotate(glm::mat4(1.0f), 0.f, glm::vec3(0.0f, 1.0f, 0.0f));

			glm::vec3 const scaleVec = glm::vec3(1.f);
			glm::mat4 const scalingMatrix = glm::scale(glm::mat4(1.0f), scaleVec);

			glm::mat4 const modelMatrix = translationMatrix * rotationMatrix * scalingMatrix;

			ferrariPositionVec.emplace_back(modelMatrix);
		}
	}

	vkInit::TextureInBundle textureIn{};
	textureIn.CommandBuffer = m_MainCommandBuffer;
	textureIn.Queue = m_GraphicsQueue;
	textureIn.Device = m_Device;
	textureIn.PhysicalDevice = m_PhysicalDevice;
	textureIn.DescriptorSetLayout = m_DescriptorSetLayoutMesh;
	textureIn.DescriptorPool = m_DescriptorPoolMesh;

	textureIn.FileName = "Resources/ferrari_diffuse.jpg";
	m_InstancedScene3DUPtr->AddMesh(std::move(std::make_unique<ave::InstancedMesh<V3D>>(meshIn, ferrariVertexVec, ferrariIndexVec, ferrariPositionVec, textureIn)));

	//////////////////////////
	//std::vector<V3D> vehicleVertexVec{};
	//std::vector<uint32_t> vehicleIndexVec{};
	//
	//vkUtil::ParseOBJ<V3D>("Resources/vehicle.obj", vehicleVertexVec, vehicleIndexVec, true);
	//std::vector<glm::mat4> vehiclePositionVec{};
	//
	//for (int rowIdx = 0; rowIdx < numRows; ++rowIdx) 
	//{
	//	for (int colIdx = 0; colIdx >= -numCols + 1; --colIdx)
	//	{
	//		glm::mat4 const translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(colIdx * spacingX, 50.0f, rowIdx* spacingY));
	//
	//		glm::mat4 const rotationMatrix = glm::rotate(glm::mat4(1.0f), 0.f, glm::vec3(0.0f, 0.0f, 1.0f));
	//
	//		glm::vec3 scaleVec = glm::vec3(1.f);
	//		glm::mat4 const scalingMatrix = glm::scale(glm::mat4(1.0f), scaleVec);
	//
	//		glm::mat4 const modelMatrix = translationMatrix * rotationMatrix * scalingMatrix;
	//
	//		vehiclePositionVec.emplace_back(modelMatrix);
	//	}
	//}
	//
	//textureIn.FileName = "Resources/vehicle_diffuse.png";
	//m_InstancedScene3DUPtr->AddMesh(std::move(std::make_unique<ave::InstancedMesh<V3D>>(meshIn, vehicleVertexVec, vehicleIndexVec, vehiclePositionVec, textureIn)));
}

void ave::VulkanEngine::PrepareFrame(uint32_t imgIdx)
{
	auto& swapchainFrame = m_SwapchainFrameVec[imgIdx];

	m_CameraUPtr->Update();

	swapchainFrame.VPMatrix.ViewMatrix = m_CameraUPtr->GetViewMatrix();
	swapchainFrame.VPMatrix.ProjectionMatrix = m_CameraUPtr->GetProjectionMatrix();
	memcpy(swapchainFrame.VPWriteLocationPtr, &swapchainFrame.VPMatrix, sizeof(vkUtil::UBO));

	static bool pressedFThisFrame{ false };
	static bool pressedVThisFrame{ false };
	static bool pressedRThisFrame{ false };
	static bool pressedTThisFrame{ false };
	if (glfwGetKey(m_WindowPtr, GLFW_KEY_F) == GLFW_PRESS)
	{
		if (not pressedFThisFrame)
		{
			pressedFThisFrame = true;
			m_InstancedScene3DUPtr->AddInstanceToMesh(0);
		}
	}
	else if (glfwGetKey(m_WindowPtr, GLFW_KEY_F) == GLFW_RELEASE)
	{
		pressedFThisFrame = false;
	}
	if (glfwGetKey(m_WindowPtr, GLFW_KEY_V) == GLFW_PRESS)
	{
		if (not pressedVThisFrame)
		{
			pressedVThisFrame = true;
			m_InstancedScene3DUPtr->AddInstanceToMesh(1);
		}
	}
	else if (glfwGetKey(m_WindowPtr, GLFW_KEY_V) == GLFW_RELEASE)
	{
		pressedVThisFrame = false;
	}
	if (glfwGetKey(m_WindowPtr, GLFW_KEY_R) == GLFW_PRESS)
	{
		if (not pressedRThisFrame)
		{
			pressedRThisFrame = true;
			m_InstancedScene3DUPtr->RemoveInstanceFromMesh(0, 0);
		}
	}
	else if (glfwGetKey(m_WindowPtr, GLFW_KEY_R) == GLFW_RELEASE)
	{
		pressedRThisFrame = false;
	}
	if (glfwGetKey(m_WindowPtr, GLFW_KEY_T) == GLFW_PRESS)
	{
		if (not pressedTThisFrame)
		{
			pressedTThisFrame = true;
			m_InstancedScene3DUPtr->RemoveInstanceFromMesh(1, 0);
		}
	}
	else if (glfwGetKey(m_WindowPtr, GLFW_KEY_T) == GLFW_RELEASE)
	{
		pressedTThisFrame = false;
	}

	int idx{};
	for (auto const& worldMatrix : m_InstancedScene3DUPtr->GetWorldMatrices())
	{
		swapchainFrame.WMatrixVec[idx++] = worldMatrix;
	}
	memcpy(swapchainFrame.WBufferWriteLocationPtr, swapchainFrame.WMatrixVec.data(), idx * sizeof(glm::mat4));

	swapchainFrame.WriteDescriptorSet();
}

void ave::VulkanEngine::CreateFrameBuffers()
{
	vkInit::FrameBufferInBundle frameBufferIn;
	frameBufferIn.Device = m_Device;
	frameBufferIn.RenderPass = m_RenderPassUPtr->GetRenderPass();
	frameBufferIn.SwapchainExtent = m_SwapchainExtent;

	vkInit::CreateFrameBuffers(frameBufferIn, m_SwapchainFrameVec);
}

void ave::VulkanEngine::CreateFrameResources()
{
	vkInit::DescriptorSetLayoutData setLayoutData;
	setLayoutData.Count = 1;
	setLayoutData.TypeVec.emplace_back(vk::DescriptorType::eUniformBuffer);
	m_DescriptorPoolFrame = vkInit::CreateDescriptorPool(m_Device, static_cast<uint32_t>(m_SwapchainFrameVec.size()), setLayoutData);

	for (auto& frame : m_SwapchainFrameVec)
	{
		frame.InFlightFence = vkInit::CreateFence(m_Device);
		frame.SemaphoreImageAvailable = vkInit::CreateSemaphore(m_Device);
		frame.SemaphoreRenderingFinished = vkInit::CreateSemaphore(m_Device);

		frame.CreateDescriptorResources(100'000);
		frame.DescriptorSet = vkInit::CreateDescriptorSet(m_Device, m_DescriptorPoolFrame, m_DescriptorSetLayoutFrame);
	}
}

void ave::VulkanEngine::CreateDescriptorSetLayouts()
{
	vkInit::DescriptorSetLayoutData setLayoutDataFrame;
	setLayoutDataFrame.Count = 2;
	setLayoutDataFrame.IndexVec.emplace_back(0);
	setLayoutDataFrame.TypeVec.emplace_back(vk::DescriptorType::eUniformBuffer);
	setLayoutDataFrame.CountVec.emplace_back(1);
	setLayoutDataFrame.StageFlagVec.emplace_back(vk::ShaderStageFlagBits::eVertex);

	setLayoutDataFrame.IndexVec.emplace_back(1);
	setLayoutDataFrame.TypeVec.emplace_back(vk::DescriptorType::eStorageBuffer);
	setLayoutDataFrame.CountVec.emplace_back(1);
	setLayoutDataFrame.StageFlagVec.emplace_back(vk::ShaderStageFlagBits::eVertex);

	m_DescriptorSetLayoutFrame = vkInit::CreateDescriptorSetLayout(m_Device, setLayoutDataFrame);

	vkInit::DescriptorSetLayoutData setLayoutDataMesh;
	setLayoutDataMesh.Count = 1;
	setLayoutDataMesh.IndexVec.emplace_back(0);
	setLayoutDataMesh.TypeVec.emplace_back(vk::DescriptorType::eCombinedImageSampler);
	setLayoutDataMesh.CountVec.emplace_back(1);
	setLayoutDataMesh.StageFlagVec.emplace_back(vk::ShaderStageFlagBits::eFragment);

	m_DescriptorSetLayoutMesh = vkInit::CreateDescriptorSetLayout(m_Device, setLayoutDataMesh);
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
		std::cout << systemError.what() << "\n";
	}

	m_RenderPassUPtr->BeginRenderPass(commandBuffer, m_SwapchainFrameVec[imageIndex].Framebuffer, m_SwapchainExtent);
	
	std::int64_t drawnInstances{};

	m_Pipeline3DUPtr->Record(commandBuffer, m_SwapchainFrameVec[imageIndex].Framebuffer, m_SwapchainExtent, m_SwapchainFrameVec[imageIndex].DescriptorSet);

	drawnInstances += m_InstancedScene3DUPtr->Draw(commandBuffer, m_Pipeline3DUPtr->GetPipelineLayout(), drawnInstances);

	m_RenderPassUPtr->EndRenderPass(commandBuffer);

	try
	{
		commandBuffer.end();
	}
	catch (const vk::SystemError& systemError)
	{
		std::cout << systemError.what() << "\n";
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
	vkInit::CreateFrameCommandBuffers(commandBufferIn);
}

void ave::VulkanEngine::DestroySwapchain()
{
	for (auto& frame : m_SwapchainFrameVec)
	{
		frame.Destroy();
	}
	m_Device.destroyDescriptorPool(m_DescriptorPoolFrame);	

	m_Device.destroySwapchainKHR(m_Swapchain);
}

void ave::VulkanEngine::PrintKeyBindings()
{
	std::cout << "=======================================================" << std::endl;
	std::cout << "|                     Bindings                        |" << std::endl;
	std::cout << "=======================================================" << std::endl;
	std::cout << "| Key / Mouse          | Action                       |" << std::endl;
	std::cout << "-------------------------------------------------------" << std::endl;
	std::cout << "| F                    | Add an instance to the       |" << std::endl;
	std::cout << "|                      | ferrari mesh                 |" << std::endl;
	std::cout << "| V                    | Add an instance to the       |" << std::endl;
	std::cout << "|                      | spaceship mesh               |" << std::endl;
	std::cout << "| R                    | Remove an instance from the  |" << std::endl;
	std::cout << "|                      | ferrari mesh                 |" << std::endl;
	std::cout << "| T                    | Remove an instance from the  |" << std::endl;
	std::cout << "|                      | spaceship mesh               |" << std::endl;
	std::cout << "| LEFT SHIFT           | Increase translation speed   |" << std::endl;
	std::cout << "|                      | by 5x                        |" << std::endl;
	std::cout << "| W or UP              | Move forward                 |" << std::endl;
	std::cout << "| S or DOWN            | Move backward                |" << std::endl;
	std::cout << "| D or RIGHT           | Move right                   |" << std::endl;
	std::cout << "| A or LEFT            | Move left                    |" << std::endl;
	std::cout << "| Right + Left Mouse   | Move up/down                 |" << std::endl;
	std::cout << "| (Hold)               |                              |" << std::endl;
	std::cout << "| Right Mouse (Hold)   | Rotate view                  |" << std::endl;
	std::cout << "=======================================================" << std::endl;
	std::cout << std::endl;

}
