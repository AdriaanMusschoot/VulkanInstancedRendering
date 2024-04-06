#include "VulkanEngine.h"
#include <memory>

int main() 
{
	std::unique_ptr vulkanEngineUPtr{ std::make_unique<ave::VulkanEngine>() };

	return 0;
}