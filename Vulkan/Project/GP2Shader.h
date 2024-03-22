#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
#include <string>

namespace amu
{

    class Shader
    {
    public:
        Shader(const std::string& vertexShaderFile, const std::string& fragmentShaderFile)
    		: m_VertexShaderFile(vertexShaderFile)
    		, m_FragmentShaderFile(fragmentShaderFile)
        {}
    
        ~Shader() = default;
        std::vector<VkPipelineShaderStageCreateInfo>& GetShaderStages() { return m_ShaderStages; }
    
        void Initialize(const VkDevice& vkDevice);
        void DestroyShaderModules();
    
        VkPipelineShaderStageCreateInfo CreateFragmentShaderInfo();
        VkPipelineShaderStageCreateInfo CreateVertexShaderInfo();
        VkPipelineVertexInputStateCreateInfo CreateVertexInputStateInfo();
        VkPipelineInputAssemblyStateCreateInfo CreateInputAssemblyStateInfo();
        VkShaderModule CreateShaderModule(const std::vector<char>& code);
    private:
        VkDevice m_VkDevice;
        std::string m_VertexShaderFile;
        std::string m_FragmentShaderFile;
    
        std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages;
    
        VkVertexInputBindingDescription m_VertexInputBindingDescription;
        std::vector<VkVertexInputAttributeDescription> m_VertexInputAttributeDescriptors;
    
        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;
        Shader(const Shader&&) = delete;
        Shader& operator=(const Shader&&) = delete;
    };

}