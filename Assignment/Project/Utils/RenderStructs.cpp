#include "RenderStructs.h"

vk::VertexInputBindingDescription vkUtil::GetPosColBindingDescription2D()
{
	vk::VertexInputBindingDescription bindingDescription;
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(Vertex2D);
	bindingDescription.inputRate = vk::VertexInputRate::eVertex;

	return bindingDescription;
}

std::vector<vk::VertexInputAttributeDescription> vkUtil::GetPosColAttributeDescription2D()
{
	std::vector<vk::VertexInputAttributeDescription> attributeDescriptionVec{};

	attributeDescriptionVec.emplace_back(vk::VertexInputAttributeDescription{ 0, 0, vk::Format::eR32G32Sfloat, 0 });
	attributeDescriptionVec.emplace_back(vk::VertexInputAttributeDescription{ 1, 0, vk::Format::eR32G32B32Sfloat, sizeof(Vertex2D::Position) });

	return attributeDescriptionVec;
}
