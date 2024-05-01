#ifndef VK_MESH_H
#define VK_MESH_H	
#include "Engine/Configuration.h"
#include "Utils/Buffer.h"
#include "Utils/RenderStructs.h"
#include "Utils/FileReader.h"
#include <unordered_map>
#include "Rendering/Image.h"

namespace ave
{

	struct MeshInBundle
	{
		const vk::Queue& GraphicsQueue; 
		const vk::CommandBuffer& MainCommandBuffer;
	};

	template <typename VertexStruct>
	class Mesh final
	{
	public:
		Mesh(const vk::Device& device, const vk::PhysicalDevice& physicalDevice, const vkInit::TextureInBundle& texIn)
			: m_Device{ device }
			, m_PhysicalDevice{ physicalDevice }
			, m_TextureUPtr{ std::make_unique<vkInit::Texture>(texIn) }
		{

		}
		Mesh(const vk::Device& device, const vk::PhysicalDevice& physicalDevice, const MeshInBundle& in, const vkInit::TextureInBundle& texIn, const std::string& filePath, bool flipWinding)
			: Mesh::Mesh(device, physicalDevice, texIn)
		{
			if (not vkUtil::ParseOBJ(filePath, m_VertexVec, m_IndexVec, flipWinding))
			{
				std::cout << "Failed to load file\n";
			}
			InitializeIndexBuffer(in);
			InitializeVertexBuffer(in);
		}
		~Mesh()
		{
			m_Device.destroyBuffer(m_VertexBuffer.Buffer);
			m_Device.freeMemory(m_VertexBuffer.BufferMemory);
			m_Device.destroyBuffer(m_IndexBuffer.Buffer);
			m_Device.freeMemory(m_IndexBuffer.BufferMemory);
		}

		Mesh(const Mesh& other) = delete;
		Mesh(Mesh&& other) = delete;
		Mesh& operator=(const Mesh& other) = delete;
		Mesh& operator=(Mesh&& other) = delete;

		void AddVertex(const VertexStruct& vertex)
		{
			m_VertexVec.emplace_back(vertex);
		}
		void InitializeVertexBuffer(const MeshInBundle& in)
		{
			vkUtil::BufferInBundle inBundle{};
			inBundle.Device = m_Device;
			inBundle.PhysicalDevice = m_PhysicalDevice;
			inBundle.Size = sizeof(VertexStruct) * m_VertexVec.size();
			inBundle.UsageFlags = vk::BufferUsageFlagBits::eTransferSrc;
			inBundle.MemoryPropertyFlags = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

			vkUtil::DataBuffer stagingBuffer = vkUtil::CreateBuffer(inBundle);

			void* memoryLocation{ m_Device.mapMemory(stagingBuffer.BufferMemory, 0, inBundle.Size) };
			memcpy(memoryLocation, m_VertexVec.data(), inBundle.Size);
			m_Device.unmapMemory(stagingBuffer.BufferMemory);

			inBundle.UsageFlags = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer;
			inBundle.MemoryPropertyFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;
			m_VertexBuffer = vkUtil::CreateBuffer(inBundle);

			vkUtil::CopyBuffer(stagingBuffer, m_VertexBuffer, inBundle.Size, in.GraphicsQueue, in.MainCommandBuffer);

			m_Device.destroyBuffer(stagingBuffer.Buffer);
			m_Device.freeMemory(stagingBuffer.BufferMemory);
		}

		void AddIndex(uint32_t idx)
		{
			m_IndexVec.emplace_back(idx);
		}
		void InitializeIndexBuffer(const MeshInBundle& in)
		{
			vkUtil::BufferInBundle inBundle{};
			inBundle.Device = m_Device;
			inBundle.PhysicalDevice = m_PhysicalDevice;
			inBundle.Size = sizeof(uint32_t) * m_IndexVec.size();
			inBundle.UsageFlags = vk::BufferUsageFlagBits::eTransferSrc;
			inBundle.MemoryPropertyFlags = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

			vkUtil::DataBuffer stagingBuffer = vkUtil::CreateBuffer(inBundle);

			void* memoryLocation{ m_Device.mapMemory(stagingBuffer.BufferMemory, 0, inBundle.Size) };
			memcpy(memoryLocation, m_IndexVec.data(), inBundle.Size);
			m_Device.unmapMemory(stagingBuffer.BufferMemory);

			inBundle.UsageFlags = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer;
			inBundle.MemoryPropertyFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;
			m_IndexBuffer = vkUtil::CreateBuffer(inBundle);

			vkUtil::CopyBuffer(stagingBuffer, m_IndexBuffer, inBundle.Size, in.GraphicsQueue, in.MainCommandBuffer);

			m_Device.destroyBuffer(stagingBuffer.Buffer);
			m_Device.freeMemory(stagingBuffer.BufferMemory);
		}

		void SetWorldMatrix(const glm::mat4& worldMatrix)
		{
			m_WorldMatrix = worldMatrix;
		}

		void PushWorldMatrix(const vk::CommandBuffer& commandBuffer, const vk::PipelineLayout& pipelineLayout)
		{
			commandBuffer.pushConstants(pipelineLayout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(glm::mat4), &m_WorldMatrix);
		}
		void BindBuffers(const vk::CommandBuffer& commandBuffer)
		{
			vk::Buffer vertexBufferArr[]{ m_VertexBuffer.Buffer };
			vk::DeviceSize offsetArr[]{ 0 };
			commandBuffer.bindVertexBuffers(0, 1, vertexBufferArr, offsetArr);
			commandBuffer.bindIndexBuffer(m_IndexBuffer.Buffer, 0, vk::IndexType::eUint32);
		}
		void SetTexture(const vk::CommandBuffer& commandBuffer, const vk::PipelineLayout& pipelineLayout)
		{
			if (m_TextureUPtr)
			{
				m_TextureUPtr->Apply(commandBuffer, pipelineLayout);
			}
		}
		void Draw(const vk::CommandBuffer& commandBuffer) const
		{
			commandBuffer.drawIndexed(static_cast<uint32_t>(m_IndexVec.size()), 1, 0, 0, 0);
		}
	private:
		std::vector<VertexStruct> m_VertexVec;
		vkUtil::DataBuffer m_VertexBuffer;

		std::vector<uint32_t> m_IndexVec;
		vkUtil::DataBuffer m_IndexBuffer;

		vk::Device m_Device;
		vk::PhysicalDevice m_PhysicalDevice;

		glm::mat4 m_WorldMatrix{ 1.0f };	
		std::unique_ptr<vkInit::Texture> m_TextureUPtr{ nullptr };
	};
}

#endif
