#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
#include <string>
#include <array>
class GP2Shader
{
public:
    GP2Shader(const std::string& vertexShaderFile, const std::string& fragmentShaderFile)
		: m_VertexShaderFile(vertexShaderFile)
		, m_FragmentShaderFile(fragmentShaderFile)
    {}

    ~GP2Shader() = default;
    std::vector<VkPipelineShaderStageCreateInfo>& GetShaderStages() { return m_ShaderStages; }

    void Initialize(const VkDevice& vkDevice);
    void DestroyShaderModules(const VkDevice& vkDevice);

    VkPipelineShaderStageCreateInfo CreateFragmentShaderInfo(const VkDevice& vkDevice);
    VkPipelineShaderStageCreateInfo CreateVertexShaderInfo(const VkDevice& vkDevice);
    VkPipelineVertexInputStateCreateInfo CreateVertexInputStateInfo();
    VkPipelineInputAssemblyStateCreateInfo CreateInputAssemblyStateInfo();
    VkShaderModule CreateShaderModule(const VkDevice& vkDevice, const std::vector<char>& code);
private:

    std::string m_VertexShaderFile;
    std::string m_FragmentShaderFile;

    std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages;

    VkVertexInputBindingDescription m_VertexInputBindingDescription;
    std::vector<VkVertexInputAttributeDescription> m_VertexInputAttributeDescriptors;

    GP2Shader(const GP2Shader&) = delete;
    GP2Shader& operator=(const GP2Shader&) = delete;
    GP2Shader(const GP2Shader&&) = delete;
    GP2Shader& operator=(const GP2Shader&&) = delete;
};