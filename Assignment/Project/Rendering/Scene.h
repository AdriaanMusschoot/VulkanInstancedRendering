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

		void AddMesh(const ave::Mesh& mesh);

		Scene(const Scene& other) = delete;
		Scene(Scene&& other) = delete;
		Scene& operator=(const Scene& other) = delete;
		Scene& operator=(Scene&& other) = delete;

		const std::vector<glm::vec3>& GetTrianglePositions() const;
	private:
		std::vector<glm::vec3> m_TrianglePositionVec;
		std::vector<ave::Mesh> m_MeshVec;
	};

}

#endif