#include "GP2Scene.h"

void amu::Scene::CreateScene(VkPhysicalDevice physicalDevice, VkDevice device)
{
	m_VkPhysicalDevice = physicalDevice;
	m_VkDevice = device;
}

void amu::Scene::Render(VkCommandBuffer commandBuffer)
{
	for (const auto& mesh : m_MeshVec)
	{
		mesh.Draw(commandBuffer);
	}
}

void amu::Scene::AddMesh(Mesh&& mesh)
{
	m_MeshVec.emplace_back(std::move(mesh));
}

void amu::Scene::Destroy()
{
	m_MeshVec.clear();
}