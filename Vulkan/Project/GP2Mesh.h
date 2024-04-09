#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "vulkan/vulkan.h"
#include "GP2CommandPool.h"
#include "GP2DataBuffer.h"
#include "vulkanbase/VulkanUtil.h"

namespace amu
{
	template <VertexConcept VS>
	class Mesh
	{
	public:
		void InitializeVertexBuffers(VkQueue graphicsQueue, CommandPool& commandPool)
		{
			m_IndexBuffer = std::make_unique<DataBuffer>(m_VkDevice, m_VkPhysicalDevice);
			m_VertexBuffer = std::make_unique<DataBuffer>(m_VkDevice, m_VkPhysicalDevice);

			m_IndexBuffer->InitializeBuffer(
				m_IndexVec.data(),
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				sizeof(m_IndexVec[0]) * m_IndexVec.size(),
				graphicsQueue,
				commandPool);

			m_VertexBuffer->InitializeBuffer(
				m_VertexVec.data(),
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				sizeof(VS) * m_VertexVec.size(),
				graphicsQueue,
				commandPool);
		}
		void Draw(const VkCommandBuffer& commandBuffer) const
		{
			m_VertexBuffer->BindAsVertexBuffer(commandBuffer);
			m_IndexBuffer->BindAsIndexBuffer(commandBuffer);
			vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_IndexVec.size()), 1, 0, 0, 0);
		}
		
		Mesh(VkPhysicalDevice physicalDevice, VkDevice device)
			: m_VkPhysicalDevice{ physicalDevice }
			, m_VkDevice{ device }
		{}

		void AddVertex(VS&& vertex)
		{
			m_VertexVec.emplace_back(std::move(vertex));
		}

		void AddIndex(int&& index)
		{
			m_IndexVec.emplace_back(std::move(index));
		}
	private:
		std::vector<VS> m_VertexVec{};
		std::vector<uint16_t> m_IndexVec{};

		std::unique_ptr<DataBuffer> m_VertexBuffer;
		std::unique_ptr<DataBuffer> m_IndexBuffer;

		VkDevice m_VkDevice;
		VkPhysicalDevice m_VkPhysicalDevice;
	};

}