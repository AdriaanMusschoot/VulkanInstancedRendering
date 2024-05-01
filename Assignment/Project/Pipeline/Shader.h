#ifndef VK_SHADER_H
#define VK_SHADER_H
#include "Engine/Configuration.h"

namespace vkUtil
{

	std::vector<char> ReadFile(const std::string& fileName);

	vk::ShaderModule CreateModule(const vk::Device& device, const std::string& fileName);
}


#endif