#ifndef VK_SCENE_H
#define VK_SCENE_H
#include "Engine/Configuration.h"
#include "Mesh.h"

namespace ave
{

	class Scene final
	{
	public:
		Scene();
		~Scene();

		void AddMesh(std::unique_ptr<ave::Mesh> meshUPtr);

		void Draw(const vk::CommandBuffer& commandBuffer);

		Scene(const Scene& other) = delete;
		Scene(Scene&& other) = delete;
		Scene& operator=(const Scene& other) = delete;
		Scene& operator=(Scene&& other) = delete;

	private:
		std::vector<std::unique_ptr<ave::Mesh>> m_MeshVec;
	};

}

#endif