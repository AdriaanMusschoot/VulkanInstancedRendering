#ifndef AVE_INSTANCED_SCENE_H
#define AVE_INSTANCED_SCENE_H
#include "Engine/Configuration.h"
#include "InstancedMesh.h"

namespace ave
{
	template<vkUtil::Vertex VertexStruct>
	class InstancedScene final
	{
	public:
		InstancedScene() = default;
		~InstancedScene() = default;

		void AddMesh(std::unique_ptr<ave::InstancedMesh<VertexStruct>> meshUPtr)
		{
			m_InstancedMeshUPtrVec.emplace_back(std::move(meshUPtr));
		}

		std::vector<glm::mat4> const& GetWorldMatrices()
		{
			if (m_DirtyFlagWorldMatrices)
			{
				m_WorldMatricesVec.clear();
				m_WorldMatricesVec.reserve(100'000);

				int idx{};
				for (const auto& mesh : m_InstancedMeshUPtrVec)
				{
					for (const auto& worldMatrix : mesh->GetPositions())
					{
						m_WorldMatricesVec.emplace_back(worldMatrix);
					}
				}
				m_DirtyFlagWorldMatrices = false;
			}
			return m_WorldMatricesVec;
		}

		std::int64_t Draw(vk::CommandBuffer const& commandBuffer, vk::PipelineLayout const& pipelineLayout, std::int64_t const& instancesDrawn)
		{
			std::int64_t offset{ instancesDrawn };
			for (const auto& mesh : m_InstancedMeshUPtrVec)
			{
				mesh->Draw(commandBuffer, pipelineLayout, offset);
				offset += std::ssize(mesh->GetPositions());
			}
			return offset;
		}

		InstancedScene(InstancedScene const& other) = delete;
		InstancedScene(InstancedScene&& other) = delete;
		InstancedScene& operator=(InstancedScene const& other) = delete;
		InstancedScene& operator=(InstancedScene&& other) = delete;

		void RotateAllInstancesMesh(int meshIdx, float angle, glm::vec3 const& axis)
		{
			for (int idx{}; idx < m_InstancedMeshUPtrVec[meshIdx]->GetInstanceCount(); ++idx)
			{
				m_InstancedMeshUPtrVec[meshIdx]->RotateInstance(idx, angle, axis);
			}
			
			m_DirtyFlagWorldMatrices = true;
		}

		void ScaleMeshInstance(int meshIdx, int instanceIdx, glm::vec3 const& scaleVec)
		{
			m_InstancedMeshUPtrVec[meshIdx]->ScaleInstance(instanceIdx, scaleVec);
		}

	private:
		std::vector<std::unique_ptr<ave::InstancedMesh<VertexStruct>>> m_InstancedMeshUPtrVec;

		std::vector<glm::mat4> m_WorldMatricesVec;
		bool m_DirtyFlagWorldMatrices{ true };
	};
}

#endif //AVE_INSTANCED_SCENE_H