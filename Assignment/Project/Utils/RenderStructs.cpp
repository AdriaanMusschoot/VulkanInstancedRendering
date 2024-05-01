#include "RenderStructs.h"

std::vector<vk::VertexInputBindingDescription> vkUtil::GetBindingDescription2D()
{
	std::vector<vk::VertexInputBindingDescription> bindingDescriptionVec;
	bindingDescriptionVec.emplace_back(vk::VertexInputBindingDescription{ 0, sizeof(Vertex2D), vk::VertexInputRate::eVertex });

	return bindingDescriptionVec;
}

std::vector<vk::VertexInputAttributeDescription> vkUtil::GetAttributeDescription2D()
{
	std::vector<vk::VertexInputAttributeDescription> attributeDescriptionVec{};

	attributeDescriptionVec.emplace_back(vk::VertexInputAttributeDescription{ 0, 0, vk::Format::eR32G32Sfloat, 0 });
	attributeDescriptionVec.emplace_back(vk::VertexInputAttributeDescription{ 1, 0, vk::Format::eR32G32B32Sfloat, sizeof(Vertex2D::Position) });
	attributeDescriptionVec.emplace_back(vk::VertexInputAttributeDescription{ 2, 0, vk::Format::eR32G32Sfloat, sizeof(Vertex2D::Position) + sizeof(Vertex2D::Color) });

	return attributeDescriptionVec;
}

std::vector<vk::VertexInputBindingDescription> vkUtil::GetBindingDescription3D()
{
	std::vector<vk::VertexInputBindingDescription> bindingDescriptionVec;
	bindingDescriptionVec.emplace_back(vk::VertexInputBindingDescription{ 0, sizeof(Vertex3D), vk::VertexInputRate::eVertex });

	return bindingDescriptionVec;
}

std::vector<vk::VertexInputAttributeDescription> vkUtil::GetAttributeDescription3D()
{
	std::vector<vk::VertexInputAttributeDescription> attributeDescriptionVec{};

	attributeDescriptionVec.emplace_back(vk::VertexInputAttributeDescription{ 0, 0, vk::Format::eR32G32B32Sfloat, 0 });
	attributeDescriptionVec.emplace_back(vk::VertexInputAttributeDescription{ 1, 0, vk::Format::eR32G32B32Sfloat, sizeof(Vertex3D::Position) });
	attributeDescriptionVec.emplace_back(vk::VertexInputAttributeDescription{ 2, 0, vk::Format::eR32G32B32Sfloat, sizeof(Vertex3D::Position) + sizeof(Vertex3D::Normal) });

	return attributeDescriptionVec;
}
