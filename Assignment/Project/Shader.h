#ifndef VK_SHADER_H
#define VK_SHADER_H
#include "Configuration.h"

namespace vkUtil
{

	std::vector<char> ReadFile(const std::string& fileName, bool isDebugging)
	{
		std::ifstream file{ fileName, std::ios::ate | std::ios::binary };

		if (isDebugging && not file.is_open())
		{
			std::cout << "Failed to open: \"" << fileName << "\'\n";
		}

		//this will return the location of the pointer and since we start at the end its the size of the file
		const size_t fileSize{ static_cast<size_t>(file.tellg()) };

		std::vector<char> charVec;
		charVec.reserve(fileSize);

		//reset the file pointer to the start of the file
		file.seekg(0);
		file.read(charVec.data(), fileSize);

		file.close();
		return charVec;
	}

	vk::ShaderModule CreateModule(const vk::Device& device, const std::string& fileName, bool isDebugging)
	{
		std::vector<char> sourceCodeVec{ ReadFile(fileName, isDebugging) };

		vk::ShaderModuleCreateInfo moduleInfo{};
		moduleInfo.flags = vk::ShaderModuleCreateFlags{};
		moduleInfo.codeSize = sourceCodeVec.size();
		moduleInfo.pCode = reinterpret_cast<const uint32_t*>(sourceCodeVec.data());

		try
		{
			return device.createShaderModule(moduleInfo);
		}
		catch (const vk::SystemError& systemError)
		{
			if (isDebugging)
			{
				std::cout << "Shader module (" << fileName << ") creation failure\n";
				std::cout << systemError.what() << "\n";
			}
		}
	}

}


#endif