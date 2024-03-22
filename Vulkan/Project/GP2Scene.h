#pragma once
#include "vulkanbase/VulkanUtil.h"
#include "GP2Mesh.h"

namespace amu
{
	class Scene
	{
	public:
		void CreateScene(VkPhysicalDevice physicalDevice, VkDevice device);

		void Render(VkCommandBuffer commandBuffer);
		void AddMesh(Mesh&& mesh);

		void Destroy();
	private:
		std::vector<Mesh> m_MeshVec;

		VkPhysicalDevice m_VkPhysicalDevice;
		VkDevice m_VkDevice;
	};
}