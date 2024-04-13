#ifndef VK_SCENE_H
#define VK_SCENE_H
#include "Engine/Configuration.h"
#include "Mesh.h"

namespace ave
{
	template<typename VertexStruct>
	class Scene final
	{
	public:
		Scene() = default;
		~Scene() = default;

		void AddMesh(std::unique_ptr<ave::Mesh<VertexStruct>> meshUPtr)
		{
			m_MeshVec.emplace_back(std::move(meshUPtr));
		}

		void Draw(const vk::CommandBuffer& commandBuffer, const vk::PipelineLayout& pipelineLayout)
		{
			for (const auto& mesh : m_MeshVec)
			{
				mesh->PushWorldMatrix(commandBuffer, pipelineLayout);
				mesh->BindBuffers(commandBuffer);
				mesh->Draw(commandBuffer);
			}
		}

		Scene(const Scene& other) = delete;
		Scene(Scene&& other) = delete;
		Scene& operator=(const Scene& other) = delete;
		Scene& operator=(Scene&& other) = delete;

	private:
		std::vector<std::unique_ptr<ave::Mesh<VertexStruct>>> m_MeshVec;
	};

}

#endif