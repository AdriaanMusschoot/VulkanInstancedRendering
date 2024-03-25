#pragma once
#include <vector>
#include <string>
#include "vulkanbase/VulkanUtil.h"
#include "GP2DataBuffer.h"
namespace amu
{
    template <VertexConcept VS>
    class Shader
    {
    public:
        Shader(const std::string& vertexShaderFile, const std::string& fragmentShaderFile)
    		: m_VertexShaderFile(vertexShaderFile)
    		, m_FragmentShaderFile(fragmentShaderFile)
        {}
    
        ~Shader() = default;
        std::vector<VkPipelineShaderStageCreateInfo>& GetShaderStages() { return m_ShaderStages; }
    
        void Initialize(const VkDevice& vkDevice)
        {
            m_VkDevice = vkDevice;
            m_ShaderStages.push_back(CreateVertexShaderInfo());
            m_ShaderStages.push_back(CreateFragmentShaderInfo());
        }

        void DestroyShaderModules()
        {
            for (const auto& shaderStageInfo : m_ShaderStages)
            {
                vkDestroyShaderModule(m_VkDevice, shaderStageInfo.module, nullptr);
            }
            m_ShaderStages.clear();
        }
    
        VkPipelineShaderStageCreateInfo CreateFragmentShaderInfo()
        {
            std::vector<char> fragShaderCode = readFile(m_FragmentShaderFile);
            VkShaderModule fragShaderModule = CreateShaderModule(fragShaderCode);

            VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
            fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            fragShaderStageInfo.module = fragShaderModule;
            fragShaderStageInfo.pName = "main";

            return fragShaderStageInfo;
        }
        VkPipelineShaderStageCreateInfo CreateVertexShaderInfo()
        {
            std::vector<char> vertShaderCode = readFile(m_VertexShaderFile);
            VkShaderModule vertShaderModule = CreateShaderModule(vertShaderCode);

            VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
            vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
            vertShaderStageInfo.module = vertShaderModule;
            vertShaderStageInfo.pName = "main";
            return vertShaderStageInfo;
        }
        const VkPipelineVertexInputStateCreateInfo& CreateVertexInputStateInfo()
        {
            m_VertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

            m_VertexInputBindingDescription = VS::GetBindingDescriptions();
            m_VertexInputAttributeDescriptors = VS::GetAttributeDescriptions();

            m_VertexInputInfo.vertexBindingDescriptionCount = 1;
            m_VertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_VertexInputAttributeDescriptors.size());
            m_VertexInputInfo.pVertexBindingDescriptions = &m_VertexInputBindingDescription;
            m_VertexInputInfo.pVertexAttributeDescriptions = m_VertexInputAttributeDescriptors.data();
            return m_VertexInputInfo;
        }
        const VkPipelineInputAssemblyStateCreateInfo& CreateInputAssemblyStateInfo()
        {
            m_InputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            m_InputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            m_InputAssembly.primitiveRestartEnable = VK_FALSE;
            return m_InputAssembly;
        }

        VkShaderModule CreateShaderModule(const std::vector<char>& code)
        {
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
    private:
        VkDevice m_VkDevice;
        VkPhysicalDevice m_VkPhyiscalDevice;
        std::string m_VertexShaderFile;
        std::string m_FragmentShaderFile;
    
        std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages;
    
        VkVertexInputBindingDescription m_VertexInputBindingDescription;
        std::vector<VkVertexInputAttributeDescription> m_VertexInputAttributeDescriptors;
    
        VkPipelineInputAssemblyStateCreateInfo m_InputAssembly{};
        VkPipelineVertexInputStateCreateInfo m_VertexInputInfo{};

        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;
        Shader(const Shader&&) = delete;
        Shader& operator=(const Shader&&) = delete;
    };

}