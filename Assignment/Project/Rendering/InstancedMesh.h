#ifndef VK_INSTANCED_MESH_H
#define VK_INSTANCED_MESH_H
#include "Engine/Configuration.h"
#include "Utils/RenderStructs.h"
#include "Utils/Buffer.h"
#include "Rendering/Image.h"

namespace ave
{


	class InstancedMesh final
	{
	public:
		struct MeshInBundle
		{
			vk::Queue const& GraphicsQueue;
			vk::CommandBuffer const& MainCommandBuffer;
			vk::Device const& Device;
			vk::PhysicalDevice const& PhysicalDevice;
		};

		InstancedMesh(MeshInBundle const& in, std::vector<vkUtil::Vertex2D> const& vertexVec, std::vector<uint32_t> const& indexVec, std::vector<glm::vec3> const& positionVec, vkInit::TextureInBundle const& texIn);
		~InstancedMesh();

		InstancedMesh(InstancedMesh const& other) = delete;
		InstancedMesh(InstancedMesh&& other) = delete;
		InstancedMesh& operator=(InstancedMesh const& other) = delete;
		InstancedMesh& operator=(InstancedMesh&& other) = delete;

		void InitializeVertexBuffer(vk::Queue const& graphicsQueue, vk::CommandBuffer const& mainCommandBuffer);
		void InitializeIndexBuffer(vk::Queue const& graphicsQueue, vk::CommandBuffer const& mainCommandBuffer);

		void Draw(vk::CommandBuffer const& commandBuffer, vk::PipelineLayout const& pipelineLayout) const;
		static std::vector<glm::vec3> const& GetPositions();

	private:
		std::vector<vkUtil::Vertex2D> m_VertexVec;
		vkUtil::DataBuffer m_VertexBuffer;

		std::vector<uint32_t> m_IndexVec;
		vkUtil::DataBuffer m_IndexBuffer;

		vk::Device m_Device;
		vk::PhysicalDevice m_PhysicalDevice;

		std::int64_t const m_StartOffset{ 0 };
		std::int64_t const m_InstanceCount{ 0 };

		std::unique_ptr<vkInit::Texture> m_TextureUPtr{ nullptr };
		static std::vector<glm::vec3> m_PositionVec;
	};
}

#endif