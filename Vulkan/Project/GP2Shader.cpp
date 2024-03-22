#include "GP2Shader.h"
#include "vulkanbase/VulkanUtil.h"
#include "GP2Mesh.h"

void amu::Shader::Initialize(const VkDevice& vkDevice)
{
	m_VkDevice = vkDevice;
	m_ShaderStages.push_back(CreateVertexShaderInfo());
	m_ShaderStages.push_back(CreateFragmentShaderInfo());
}

void amu::Shader::DestroyShaderModules()
{
	for (const auto& shaderStageInfo : m_ShaderStages)
	{
		vkDestroyShaderModule(m_VkDevice, shaderStageInfo.module, nullptr);
	}
	m_ShaderStages.clear();
}

VkPipelineShaderStageCreateInfo amu::Shader::CreateFragmentShaderInfo() {
	std::vector<char> fragShaderCode = readFile(m_FragmentShaderFile);
	VkShaderModule fragShaderModule = CreateShaderModule(fragShaderCode);

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	return fragShaderStageInfo;
}

VkPipelineShaderStageCreateInfo amu::Shader::CreateVertexShaderInfo() {
	std::vector<char> vertShaderCode = readFile(m_VertexShaderFile);
	VkShaderModule vertShaderModule = CreateShaderModule(vertShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";
	return vertShaderStageInfo;
}

VkPipelineVertexInputStateCreateInfo amu::Shader::CreateVertexInputStateInfo()
{
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	m_VertexInputBindingDescription = amu::Mesh::Vertex::getBindingDescription();
	m_VertexInputAttributeDescriptors = amu::Mesh::Vertex::getAttributeDescriptions();

	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_VertexInputAttributeDescriptors.size());
	vertexInputInfo.pVertexBindingDescriptions = &m_VertexInputBindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = m_VertexInputAttributeDescriptors.data();
	return vertexInputInfo;
} 

VkPipelineInputAssemblyStateCreateInfo amu::Shader::CreateInputAssemblyStateInfo()
{
	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;
	return inputAssembly;
}

VkShaderModule amu::Shader::CreateShaderModule(const std::vector<char>& code) {
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(m_VkDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module!");
	}

	return shaderModule;
}