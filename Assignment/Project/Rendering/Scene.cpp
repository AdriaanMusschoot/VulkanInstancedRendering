#include "Scene.h"

ave::Scene::Scene()
{
	for (float idxX{ -1.f }; idxX < 1.0f; idxX += 0.2f)
	{
		for (float idxY{ -1.f }; idxY < 1.0f; idxY += 0.2f)
		{
			m_TrianglePositionVec.emplace_back(glm::vec3{ idxX, idxY, 0 });
		}
	}
}

ave::Scene::~Scene()
{
}

void ave::Scene::AddMesh(const ave::Mesh& mesh)
{
	//m_MeshVec.emplace_back(mesh);
}

const std::vector<glm::vec3>& ave::Scene::GetTrianglePositions() const
{
	return m_TrianglePositionVec;
}
