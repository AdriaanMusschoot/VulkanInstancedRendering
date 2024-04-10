#ifndef VK_MESH_H
#define VK_MESH_H	
#include "Engine/Configuration.h"
#include "Utils/Buffer.h"
#include "Utils/RenderStructs.h"

namespace ave
{

	class Mesh final
	{
	public:
		Mesh(const vk::Device& device, const vk::PhysicalDevice& physicalDevice);
		~Mesh();

		void AddVertex(const vkUtil::Vertex2D& vertex);
		void InitializeBuffer();

		void BindBuffer(const vk::CommandBuffer& commandBuffer);
		void Draw(const vk::CommandBuffer& commandBuffer) const;

		Mesh(const Mesh& other) = delete;
		Mesh(Mesh&& other) = delete;
		Mesh& operator=(const Mesh& other) = delete;
		Mesh& operator=(Mesh&& other) = delete;
	private:
		std::vector<vkUtil::Vertex2D> m_VertexVec;

		vkUtil::DataBuffer m_Buffer;

		vk::Device m_Device;
		vk::PhysicalDevice m_PhysicalDevice;
	};
}

#endif
