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

		void ScaleMeshInstanceOneByOne(int meshIdx)
		{
			int idx{ m_InstancedMeshUPtrVec[meshIdx]->UpdateTimer(static_cast<float>(ave::Clock::GetInstance().GetDeltaTime())) };

			auto const& currScale = m_InstancedMeshUPtrVec[meshIdx]->GetCurrentScaleValue();
			auto const& normalizeValue = m_InstancedMeshUPtrVec[meshIdx]->GetNormalizeValue();
			m_InstancedMeshUPtrVec[meshIdx]->ScaleInstance(idx, glm::vec3(currScale / normalizeValue, currScale / normalizeValue, currScale / normalizeValue));

			m_DirtyFlagWorldMatrices = true;
		}

		void TranslateMeshInstance(int meshIdx, int instanceIdx, glm::vec3 const& translationVec)
		{
			m_InstancedMeshUPtrVec[meshIdx]->TranslateInstance(instanceIdx, translationVec);

			m_DirtyFlagWorldMatrices = true;
		}

		void AddInstanceToMesh(int meshIdx)
		{
			float x{ static_cast<float>(rand() % 100 - 50) };
			float y{ static_cast<float>(rand() % 100 - 50) };
			float z{ static_cast<float>(rand() % 100 + 200) };
			glm::mat4 newWorldMatrix{ glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z)) };
			newWorldMatrix = glm::rotate(newWorldMatrix, glm::radians(x), glm::vec3(1, 0, 0));
			newWorldMatrix = glm::rotate(newWorldMatrix, glm::radians(y), glm::vec3(0, 1, 0));
			newWorldMatrix = glm::rotate(newWorldMatrix, glm::radians(z), glm::vec3(0, 0, 1));
			float scaleX{ (rand() % 10 + 5) / 15.f };
			newWorldMatrix = glm::scale(newWorldMatrix, glm::vec3(scaleX, scaleX, scaleX));
			m_InstancedMeshUPtrVec[meshIdx]->AddInstance(newWorldMatrix);

			m_DirtyFlagWorldMatrices = true;
		}
		void RemoveInstanceFromMesh(int meshIdx, int instanceIdx)
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