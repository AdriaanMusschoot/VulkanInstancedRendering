#pragma once
#include "vulkanbase/VulkanUtil.h"

namespace amu
{

	class CommandBuffer
	{
	public:
		//TODO make the command buffer owned by the commandpool maybe
		void SetCommandBuffer(const VkCommandBuffer& cmdBuffer)
		{
			m_CommandBuffer = cmdBuffer;
		}
		const VkCommandBuffer& GetCommandBuffer() const
		{
			return m_CommandBuffer;
		}
	
		void BeginCommandBuffer();
		void EndCommandBuffer();
	private:
		VkCommandBuffer m_CommandBuffer;
	};

}