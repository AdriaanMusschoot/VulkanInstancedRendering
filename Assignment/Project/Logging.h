#ifndef VK_LOGGING_H
#define VK_LOGGING_H
#include "Configuration.h"

namespace vkInit
{
	VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallBack
	(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* callBackDataPtr,
		void* userDataPtr
	)
	{
		std::cerr << "Validation layer: " << callBackDataPtr->pMessage << "\n";

		return VK_FALSE;
	}

	vk::DebugUtilsMessengerEXT CreateDebugMessenger(const vk::Instance& instance, const vk::DispatchLoaderDynamic& dLDInstance)
	{
		vk::DebugUtilsMessengerCreateInfoEXT createInfo
		{
			vk::DebugUtilsMessengerCreateFlagsEXT(),
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
			vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
			DebugCallBack,
			nullptr
		};

		return instance.createDebugUtilsMessengerEXT(createInfo, nullptr, dLDInstance);
	}
}

#endif