#include "Scene.h"

ave::Scene::Scene()
{
}

ave::Scene::~Scene()
{
}

void ave::Scene::AddMesh(std::unique_ptr<ave::Mesh> meshUPtr)
{
	meshUPtr->InitializeBuffer();
	m_MeshVec.emplace_back(std::move(meshUPtr));
}

void ave::Scene::Draw(const vk::CommandBuffer& commandBuffer)
{
	for (const auto& mesh : m_MeshVec)
	{
		mesh->BindBuffer(commandBuffer);
		mesh->Draw(commandBuffer);
	}
}
