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
	CreateScene2D();
}

ave::VulkanEngine::~VulkanEngine()
{
	m_Device.waitIdle();	

	m_RenderPassUPtr.reset();
	m_Pipeline2DUPtr.reset();
	m_Pipeline3DUPtr.reset();

	m_Device.destroyCommandPool(m_CommandPool);

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

	for (auto& frame : m_SwapchainFrameVec)
	{
		frame.Device = m_Device;
		frame.PhysicalDevice = m_PhysicalDevice;
		frame.DepthExtent = m_SwapchainExtent;

		frame.CreateDepthResources(m_IsDebugging);
	}
}

void ave::VulkanEngine::CreatePipeline()
{
	vkInit::RenderPassInBundle inRenderPass{};
	inRenderPass.Device = m_Device;
	inRenderPass.DepthFormat = m_SwapchainFrameVec[0].DepthFormat;
	inRenderPass.SwapchainImageFormat = m_SwapchainFormat;
	inRenderPass.AttachmentFlags = static_cast<vkUtil::AttachmentFlags>(vkUtil::AttachmentFlags::Color | vkUtil::AttachmentFlags::Depth);
	m_RenderPassUPtr = std::make_unique<vkInit::RenderPass>(inRenderPass, m_IsDebugging);

	vkInit::Pipeline<vkUtil::Vertex2D>::GraphicsPipelineInBundle specification2D{};
	specification2D.Device = m_Device;
	specification2D.SwapchainExtent = m_SwapchainExtent;
	specification2D.DescriptorSetLayout = m_DescriptorSetLayout;
	specification2D.VertexFilePath = "shaders/Shader2D.vert.spv";
	specification2D.FragmentFilePath = "shaders/Shader2D.frag.spv";
	specification2D.RenderPass = m_RenderPassUPtr->GetRenderPass();
	specification2D.GetBindingDescription = vkUtil::GetBindingDescription2D;
	specification2D.GetAttributeDescription = vkUtil::GetAttributeDescription2D;

	m_Pipeline2DUPtr = std::make_unique<vkInit::Pipeline<vkUtil::Vertex2D>>(specification2D, m_IsDebugging);

	vkInit::Pipeline<vkUtil::Vertex3D>::GraphicsPipelineInBundle specification3D{};
	specification3D.Device = m_Device;
	specification3D.SwapchainExtent = m_SwapchainExtent;
	specification3D.DescriptorSetLayout = m_DescriptorSetLayout;
	specification3D.VertexFilePath = "shaders/Shader3D.vert.spv";
	specification3D.FragmentFilePath = "shaders/Shader3D.frag.spv";
	specification3D.RenderPass = m_RenderPassUPtr->GetRenderPass();
	specification3D.GetBindingDescription = vkUtil::GetBindingDescription3D;
	specification3D.GetAttributeDescription = vkUtil::GetAttributeDescription3D;

	m_Pipeline3DUPtr = std::make_unique<vkInit::Pipeline<vkUtil::Vertex3D>>(specification3D, m_IsDebugging);
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

	m_Pipeline2DUPtr->SetScene(std::move(CreateScene2D()));

	m_Pipeline3DUPtr->SetScene(std::move(CreateScene3D()));

	m_CameraUPtr = std::make_unique<Camera>(m_WindowPtr, glm::vec3{ 0, 0, -150 }, 20, m_SwapchainExtent.width, m_SwapchainExtent.height);
}

std::unique_ptr<ave::Scene<vkUtil::Vertex2D>> ave::VulkanEngine::CreateScene2D()
{
	ave::MeshInBundle meshInput
	{
		m_GraphicsQueue,
		m_MainCommandBuffer
	};

	std::unique_ptr sceneUPtr{ std::make_unique<ave::Scene<vkUtil::Vertex2D>>() };
	
	std::unique_ptr RectangleMeshUPtr{ std::make_unique<ave::Mesh<vkUtil::Vertex2D>>(m_Device, m_PhysicalDevice) };
	RectangleMeshUPtr->AddVertex(vkUtil::Vertex2D{ { 0.2f, 0.0f }, { 0.0f, 1.0f, 0.0f } });
	RectangleMeshUPtr->AddVertex(vkUtil::Vertex2D{ { 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } });
	RectangleMeshUPtr->AddVertex(vkUtil::Vertex2D{ { 0.2f, 0.2f }, { 0.0f, 1.0f, 0.0f } });
	RectangleMeshUPtr->AddVertex(vkUtil::Vertex2D{ { 0.0f, 0.2f }, { 0.0f, 0.0f, 1.0f } });
	RectangleMeshUPtr->AddIndex(0);
	RectangleMeshUPtr->AddIndex(1);
	RectangleMeshUPtr->AddIndex(2);
	RectangleMeshUPtr->AddIndex(3);
	RectangleMeshUPtr->AddIndex(2);
	RectangleMeshUPtr->AddIndex(1);
		
	RectangleMeshUPtr->InitializeIndexBuffer(meshInput);
	RectangleMeshUPtr->InitializeVertexBuffer(meshInput);
	
	sceneUPtr->AddMesh(std::move(RectangleMeshUPtr));
	
	std::unique_ptr circleMeshPtr{ std::make_unique<ave::Mesh<vkUtil::Vertex2D>>(m_Device, m_PhysicalDevice) };
	
	constexpr double radius{ 0.1f };
	constexpr int nrOfPoints{ 100 };
	constexpr float centerX{ 0.1f };
	constexpr float centerY{ -0.1f };
	
	std::vector<vkUtil::Vertex2D> tempVertexVec;
	tempVertexVec.reserve(nrOfPoints);
	for (int idx{ 0 }; idx < nrOfPoints; ++idx)
	{
		double theta = 2.0 * 3.14 * idx / nrOfPoints;
		vkUtil::Vertex2D vert{};
		vert.Position.x = static_cast<float>(centerX + radius * std::cos(theta));
		vert.Position.y = static_cast<float>(centerY + radius * std::sin(theta));
	
		const float hue = static_cast<float>(idx) / static_cast<float>(nrOfPoints);
		vert.Color = glm::vec3(glm::abs(glm::cos(hue * 3.14 * 2.0f)), glm::abs(glm::sin(hue * 3.14 * 2.0f)), 0.5f);
		tempVertexVec.emplace_back(vert);
	}
	
	uint32_t vIdx{};
	for (int idx{ 0 }; idx < tempVertexVec.size(); ++idx)
	{
		if (idx < tempVertexVec.size() - 1)
		{
			circleMeshPtr->AddVertex(tempVertexVec[idx]);
			circleMeshPtr->AddIndex(vIdx);
			++vIdx;
			circleMeshPtr->AddVertex(vkUtil::Vertex2D{ glm::vec2{ centerX, centerY }, glm::vec3{ 1, 1, 1 } });
			circleMeshPtr->AddIndex(vIdx);
			++vIdx;
			circleMeshPtr->AddVertex(tempVertexVec[idx + 1]);
			circleMeshPtr->AddIndex(vIdx);
			++vIdx;
		}
		else
		{
			circleMeshPtr->AddVertex(tempVertexVec[idx]);
			circleMeshPtr->AddIndex(vIdx);
			++vIdx;
			circleMeshPtr->AddVertex(vkUtil::Vertex2D{ glm::vec2{ centerX, centerY }, glm::vec3{ 1, 1, 1 } });
			circleMeshPtr->AddIndex(vIdx);
			++vIdx;
			circleMeshPtr->AddVertex(tempVertexVec[0]);
			circleMeshPtr->AddIndex(vIdx);
			++vIdx;
		}
	}
	
	circleMeshPtr->InitializeIndexBuffer(meshInput);
	circleMeshPtr->InitializeVertexBuffer(meshInput);
	
	sceneUPtr->AddMesh(std::move(circleMeshPtr));
	
	return sceneUPtr;
}

std::unique_ptr<ave::Scene<vkUtil::Vertex3D>> ave::VulkanEngine::CreateScene3D()
{
	ave::MeshInBundle meshInput
	{
		m_GraphicsQueue,
		m_MainCommandBuffer
	};

	std::unique_ptr sceneUPtr{ std::make_unique<ave::Scene<vkUtil::Vertex3D>>() };

	const std::string fileNameVehicle{ "Resources/vehicle.obj" };
	
	std::unique_ptr vehicleMeshUPtr{ std::make_unique<ave::Mesh<vkUtil::Vertex3D>>(m_Device, m_PhysicalDevice, meshInput, fileNameVehicle)};
	
	vehicleMeshUPtr->SetWorldMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(-15.0f, 0.0f, 0.0f)));
	
	sceneUPtr->AddMesh(std::move(vehicleMeshUPtr));
	
	const std::string fileNameRaceCar{ "Resources/ferrari.obj" };
	
	std::unique_ptr raceCarUPtr{ std::make_unique<ave::Mesh<vkUtil::Vertex3D>>(m_Device, m_PhysicalDevice, meshInput, fileNameRaceCar)};
	
	raceCarUPtr->SetWorldMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(15.0f, 0.0f, 0.0f)));
	
	sceneUPtr->AddMesh(std::move(raceCarUPtr));

	return sceneUPtr;
}

void ave::VulkanEngine::PrepareFrame(uint32_t imgIdx)
{
	m_CameraUPtr->Update();

	m_SwapchainFrameVec[imgIdx].VPMatrix.ViewMatrix = m_CameraUPtr->GetViewMatrix();
	m_SwapchainFrameVec[imgIdx].VPMatrix.ProjectionMatrix = m_CameraUPtr->GetProjectionMatrix();
	memcpy(m_SwapchainFrameVec[imgIdx].VPWriteLocationPtr, &m_SwapchainFrameVec[imgIdx].VPMatrix, sizeof(vkUtil::UBO));

	m_SwapchainFrameVec[imgIdx].WriteDescriptorSet();
}

void ave::VulkanEngine::CreateFrameBuffers()
{
	vkInit::FrameBufferInBundle frameBufferIn;
	frameBufferIn.Device = m_Device;
	frameBufferIn.RenderPass = m_RenderPassUPtr->GetRenderPass();
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

		frame.CreateUBOResources();
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

	m_RenderPassUPtr->BeginRenderPass(commandBuffer, m_SwapchainFrameVec[imageIndex].Framebuffer, m_SwapchainExtent);
	
	m_Pipeline3DUPtr->Record(commandBuffer, m_SwapchainFrameVec[imageIndex].Framebuffer, m_SwapchainExtent, m_SwapchainFrameVec[imageIndex].UBODescriptorSet);

	m_Pipeline2DUPtr->Record(commandBuffer, m_SwapchainFrameVec[imageIndex].Framebuffer, m_SwapchainExtent, m_SwapchainFrameVec[imageIndex].UBODescriptorSet);

	m_RenderPassUPtr->EndRenderPass(commandBuffer);

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
		frame.Destroy();
	}
	m_Device.destroyDescriptorPool(m_DescriptorPool);	

	m_Device.destroySwapchainKHR(m_Swapchain);
}