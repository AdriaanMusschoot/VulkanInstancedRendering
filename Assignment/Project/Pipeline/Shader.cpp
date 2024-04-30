#include "Shader.h"

std::vector<char> vkUtil::ReadFile(const std::string& fileName)
{
	std::ifstream file{ fileName, std::ios::ate | std::ios::binary };

	if (not file.is_open())
	{
		std::cout << "Failed to open: \"" << fileName << "\'\n";
	}

	//this will return the location of the pointer and since we start at the end its the size of the file
	const size_t fileSize{ static_cast<size_t>(file.tellg()) };

	std::vector<char> charVec;
	charVec.resize(fileSize);

	//reset the file pointer to the start of the file
	file.seekg(0);
	file.read(charVec.data(), fileSize);

	file.close();
	return charVec;
}

vk::ShaderModule vkUtil::CreateModule(const vk::Device& device, const std::string& fileName)
{
	std::vector<char> sourceCodeVec{ ReadFile(fileName) };

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
		std::cout << "Shader module (" << fileName << ") creation failure\n";
		std::cout << systemError.what() << "\n";

		return nullptr;
	}
}
