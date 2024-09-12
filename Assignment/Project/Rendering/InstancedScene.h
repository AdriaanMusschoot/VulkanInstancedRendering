#ifndef AVE_INSTANCED_SCENE_H
#define AVE_INSTANCED_SCENE_H
#include "Engine/Configuration.h"
#include "InstancedMesh.h"
#include "Engine/Clock.h"

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

		void RemoveMesh(int idx)
		{
			m_WorldMatricesVec.erase(m_WorldMatricesVec.begin() + idx);
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

		void RotateMeshInstance(int meshIdx, float angle, glm::vec3 const& axis, int instanceIdx = 0)
		{
			m_InstancedMeshUPtrVec[meshIdx]->RotateInstance(instanceIdx, angle, axis);
			
			m_DirtyFlagWorldMatrices = true;
		}

		void ScaleMeshInstance(int meshIdx, glm::vec3 const& scaleVec, int instanceIdx = 0)
		{
			m_InstancedMeshUPtrVec[meshIdx]->ScaleInstance(instanceIdx, scaleVec);

			m_DirtyFlagWorldMatrices = true;
		}

		void TranslateMeshInstance(int meshIdx, glm::vec3 const& translationVec, int instanceIdx = 0)
		{
			m_InstancedMeshUPtrVec[meshIdx]->TranslateInstance(instanceIdx, translationVec);

			m_DirtyFlagWorldMatrices = true;
		}

		void AddInstanceToMesh(int meshIdx)
		{
			m_InstancedMeshUPtrVec[meshIdx]->AddInstance(glm::mat4(1.f));

			m_DirtyFlagWorldMatrices = true;
		}

		void RemoveInstanceFromMesh(int meshIdx, int instanceIdx = 0)
		{
			m_InstancedMeshUPtrVec[meshIdx]->RemoveInstance(instanceIdx);

			m_DirtyFlagWorldMatrices = true;
		}
	private:
		std::vector<std::unique_ptr<ave::InstancedMesh<VertexStruct>>> m_InstancedMeshUPtrVec;

		std::vector<glm::mat4> m_WorldMatricesVec;
		bool m_DirtyFlagWorldMatrices{ true };
	};
}

#endif //AVE_INSTANCED_SCENE_H