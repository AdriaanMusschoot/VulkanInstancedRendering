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

		void Draw(vk::CommandBuffer const& commandBuffer, vk::PipelineLayout const& pipelineLayout)
		{
			for (const auto& mesh : m_MeshVec)
			{
				mesh->PushWorldMatrix(commandBuffer, pipelineLayout);
				mesh->BindBuffers(commandBuffer);
				mesh->SetTexture(commandBuffer, pipelineLayout);
				mesh->Draw(commandBuffer);
			}
		}

		Scene(Scene const& other) = delete;
		Scene(Scene&& other) = delete;
		Scene& operator=(Scene const& other) = delete;
		Scene& operator=(Scene&& other) = delete;

		ave::Mesh<VertexStruct> const* const GetMesh(int idx) const
		{
			return m_MeshVec[idx].get();
		}
	private:
		std::vector<std::unique_ptr<ave::Mesh<VertexStruct>>> m_MeshVec;
	};

}

#endif