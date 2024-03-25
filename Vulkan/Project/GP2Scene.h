#pragma once
#include "vulkanbase/VulkanUtil.h"
#include "GP2Mesh.h"

namespace amu
{
	template<VertexConcept VS>
	class Scene
	{
	public:
		void CreateScene(VkPhysicalDevice physicalDevice, VkDevice device)
		{
			m_VkPhysicalDevice = physicalDevice;
			m_VkDevice = device;
		}

		void Render(VkCommandBuffer commandBuffer)
		{
			for (const auto& mesh : m_MeshVec)
			{
				mesh.Draw(commandBuffer);
			}
		}
		void AddMesh(Mesh<VS>&& mesh)
		{
			m_MeshVec.emplace_back(std::move(mesh));
		}

		void Destroy()
		{
			m_MeshVec.clear();
		}
	private:
		std::vector<Mesh<VS>> m_MeshVec;

		VkPhysicalDevice m_VkPhysicalDevice;
		VkDevice m_VkDevice;
	};

}