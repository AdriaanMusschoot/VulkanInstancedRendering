#ifndef VK_INSTANCED_MESH_H
#define VK_INSTANCED_MESH_H
#include "Engine/Configuration.h"
#include "Utils/RenderStructs.h"
#include "Utils/Buffer.h"
#include "Rendering/Image.h"

namespace ave
{

	template<typename VertexStruct>
	class InstancedMesh final
	{
	public:
		InstancedMesh(vkUtil::MeshInBundle const& in, std::vector<VertexStruct> const& vertexVec, std::vector<uint32_t> const& indexVec, std::vector<glm::vec3> const& positionVec, vkInit::TextureInBundle const& texIn)
			: m_VertexVec{ vertexVec }
			, m_IndexVec{ indexVec }
			, m_Device{ in.Device }
			, m_PhysicalDevice{ in.PhysicalDevice }
			, m_StartOffset{ std::ssize(m_PositionVec) }
			, m_InstanceCount{ std::ssize(positionVec) }
			, m_TextureUPtr{ std::make_unique<vkInit::Texture>(texIn) }
		{
			m_PositionVec.reserve(std::ssize(m_PositionVec) + std::ssize(positionVec));
			std::for_each(std::execution::seq, positionVec.begin(), positionVec.end(),
				[&](glm::vec3 const& position)
				{
					//sequential because order matters and we avoid emplacing at the same time
					m_PositionVec.emplace_back(position);
				});
			InitializeVertexBuffer(in.GraphicsQueue, in.MainCommandBuffer);
			InitializeIndexBuffer(in.GraphicsQueue, in.MainCommandBuffer);
		}

		~InstancedMesh()
		{
			m_Device.destroyBuffer(m_VertexBuffer.Buffer);
			m_Device.freeMemory(m_VertexBuffer.BufferMemory);
			m_Device.destroyBuffer(m_IndexBuffer.Buffer);
			m_Device.freeMemory(m_IndexBuffer.BufferMemory);
		}

		InstancedMesh(InstancedMesh const& other) = delete;
		InstancedMesh(InstancedMesh&& other) = delete;
		InstancedMesh& operator=(InstancedMesh const& other) = delete;
		InstancedMesh& operator=(InstancedMesh&& other) = delete;

		void InitializeVertexBuffer(vk::Queue const& graphicsQueue, vk::CommandBuffer const& mainCommandBuffer)
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

			vkUtil::CopyBuffer(stagingBuffer, m_VertexBuffer, inBundle.Size, graphicsQueue, mainCommandBuffer);

			m_Device.destroyBuffer(stagingBuffer.Buffer);
			m_Device.freeMemory(stagingBuffer.BufferMemory);
		}
		void InitializeIndexBuffer(vk::Queue const& graphicsQueue, vk::CommandBuffer const& mainCommandBuffer)
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

			vkUtil::CopyBuffer(stagingBuffer, m_IndexBuffer, inBundle.Size, graphicsQueue, mainCommandBuffer);

			m_Device.destroyBuffer(stagingBuffer.Buffer);
			m_Device.freeMemory(stagingBuffer.BufferMemory);
		}

		void Draw(vk::CommandBuffer const& commandBuffer, vk::PipelineLayout const& pipelineLayout) const
		{
			vk::Buffer vertexBufferArr[]{ m_VertexBuffer.Buffer };
			vk::DeviceSize offsetArr[]{ 0 };
			commandBuffer.bindVertexBuffers(0, 1, vertexBufferArr, offsetArr);
			commandBuffer.bindIndexBuffer(m_IndexBuffer.Buffer, 0, vk::IndexType::eUint32);

			if (m_TextureUPtr)
			{
				m_TextureUPtr->Apply(commandBuffer, pipelineLayout);
			}

			commandBuffer.drawIndexed(std::ssize(m_IndexVec), m_InstanceCount, 0, 0, m_StartOffset);
		}
		static std::vector<glm::vec3> const& GetPositions()
		{
			return m_PositionVec;
		}

	private:
		std::vector<VertexStruct> m_VertexVec;
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

	template<typename VertexStruct>
	std::vector<glm::vec3> ave::InstancedMesh<VertexStruct>::m_PositionVec = std::vector<glm::vec3>{};
}

#endif