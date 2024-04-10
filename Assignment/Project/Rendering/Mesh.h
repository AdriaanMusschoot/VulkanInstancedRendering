#ifndef VK_MESH_H
#define VK_MESH_H	
#include "Engine/Configuration.h"
#include "Utils/Buffer.h"
#include "Utils/RenderStructs.h"

namespace ave
{
	struct MeshInBundle
	{
		const vk::Queue& GraphicsQueue; 
		const vk::CommandBuffer& MainCommandBuffer;
	};

	class Mesh final
	{
	public:
		Mesh(const vk::Device& device, const vk::PhysicalDevice& physicalDevice);
		~Mesh();

		void AddVertex(const vkUtil::Vertex2D& vertex);
		void InitializeVertexBuffer(const MeshInBundle& in);

		void AddIndex(uint32_t idx);
		void InitializeIndexBuffer(const MeshInBundle& in);

		void BindBuffers(const vk::CommandBuffer& commandBuffer);
		void Draw(const vk::CommandBuffer& commandBuffer) const;

		Mesh(const Mesh& other) = delete;
		Mesh(Mesh&& other) = delete;
		Mesh& operator=(const Mesh& other) = delete;
		Mesh& operator=(Mesh&& other) = delete;
	private:
		std::vector<vkUtil::Vertex2D> m_VertexVec;
		vkUtil::DataBuffer m_VertexBuffer;

		std::vector<uint32_t> m_IndexVec;
		vkUtil::DataBuffer m_IndexBuffer;

		vk::Device m_Device;
		vk::PhysicalDevice m_PhysicalDevice;
	};
}

#endif
