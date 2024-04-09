#ifndef VK_MESH_H
#define VK_MESH_H	
#include "Engine/Configuration.h"
#include "Utils/Buffer.h"

namespace ave
{

	class Mesh final
	{
	public:
		Mesh(const vk::Device& device, const vk::PhysicalDevice& physicalDevice);
		~Mesh();

		const vkUtil::DataBuffer& GetBuffer() const;

		Mesh(const Mesh& other) = delete;
		Mesh(Mesh&& other) = delete;
		Mesh& operator=(const Mesh& other) = delete;
		Mesh& operator=(Mesh&& other) = delete;
	private:
		vkUtil::DataBuffer m_Buffer;
		vk::Device m_Device;
	};
}

#endif
