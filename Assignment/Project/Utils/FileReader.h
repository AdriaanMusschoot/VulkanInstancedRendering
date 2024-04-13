#ifndef VK_FILEREADER_OBJ_H
#define VK_FILEREADER_OBJ_H
#include "Engine/Configuration.h"

namespace vkUtil
{

	template<typename VertexStruct>
	static bool ParseOBJ(const std::string& filename, std::vector<VertexStruct>& vertexVec, std::vector<uint32_t>& indexVec, bool flipAxisAndWinding = true)
	{
		std::ifstream file{ filename };
		if (!file)
		{
			return false;
		}
	
		std::vector<glm::vec3> positionVec{};
		std::vector<glm::vec3> normalVec{};
		//std::vector<glm::vec2> uvVec{};
	
		vertexVec.clear();
		indexVec.clear();
	
		std::string sCommand;
		// start a while iteration ending when the end of file is reached (ios::eof)
		while (!file.eof())
		{
			//read the first word of the string, use the >> operator (istream::operator>>) 
			file >> sCommand;
			//use conditional statements to process the different commands	
			if (sCommand == "#")
			{
				// Ignore Comment
			}
			else if (sCommand == "v")
			{
				//Vertex
				float x, y, z;
				file >> x >> y >> z;
	
				positionVec.emplace_back(glm::vec3{ x, y, z });
			}
			//else if (sCommand == "vt")
			//
			//	// Vertex TexCoord
			//	float u, v;
			//	file >> u >> v;
			//	uvVec.emplace_back(u, 1 - v);
			//
			else if (sCommand == "vn")
			{
				// Vertex Normal
				float x, y, z;
				file >> x >> y >> z;
	
				normalVec.emplace_back(glm::vec3{ x, y, z });
			}
			else if (sCommand == "f")
			{
				//if a face is read:
				//construct the 3 vertices, add them to the vertex array
				//add three indices to the index array
				//add the material index as attibute to the attribute array
				//
				// Faces or triangles
				VertexStruct vertex{};
				size_t idxPosition, idxTexCoord, idxNormal;
	
				uint32_t tempIndexVec[3];
				for (size_t idxFace = 0; idxFace < 3; idxFace++)
				{
					// OBJ format uses 1-based arrays
					file >> idxPosition;
					vertex.Position = positionVec[idxPosition - 1];
	
					if ('/' == file.peek())//is next in buffer ==  '/' ?
					{
						file.ignore();//read and ignore one element ('/')
	
						if ('/' != file.peek())
						{
							// Optional texture coordinate
							file >> idxTexCoord;
							//vertex.uv = uvVec[iTexCoord - 1];
						}
	
						if ('/' == file.peek())
						{
							file.ignore();
	
							// Optional vertex normal
							file >> idxNormal;
							vertex.Normal = normalVec[idxNormal - 1];
						}
					}
	
					vertexVec.emplace_back(vertex);
					tempIndexVec[idxFace] = uint32_t(vertexVec.size()) - 1;
				}
	
				indexVec.emplace_back(tempIndexVec[0]);
				if (flipAxisAndWinding)
				{
					indexVec.emplace_back(tempIndexVec[2]);
					indexVec.emplace_back(tempIndexVec[1]);
				}			
				else		
				{			
					indexVec.emplace_back(tempIndexVec[1]);
					indexVec.emplace_back(tempIndexVec[2]);
				}
			}
			//read till end of line and ignore all remaining chars
			file.ignore(1000, '\n');
		}
	
		//Cheap Tangent Calculations
		//for (uint32_t i = 0; i < indexVec.size(); i += 3)
		//{
		//	uint32_t index0 = indexVec[i];
		//	uint32_t index1 = indexVec[size_t(i) + 1];
		//	uint32_t index2 = indexVec[size_t(i) + 2];
		//
		//	const glm::vec3& p0 = vertexVec[index0].position;
		//	const glm::vec3& p1 = vertexVec[index1].position;
		//	const glm::vec3& p2 = vertexVec[index2].position;
		//	//const Vector2& uv0 = vertexVec[index0].uv;
		//	//const Vector2& uv1 = vertexVec[index1].uv;
		//	//const Vector2& uv2 = vertexVec[index2].uv;
		//
		//	//const glm::vec3 edge0 = p1 - p0;
		//	//const glm::vec3 edge1 = p2 - p0;
		//	//const Vector2 diffX = Vector2(uv1.x - uv0.x, uv2.x - uv0.x);
		//	//const Vector2 diffY = Vector2(uv1.y - uv0.y, uv2.y - uv0.y);
		//	//float r = 1.f / Vector2::Cross(diffX, diffY);
		//
		//	//Vector3 tangent = (edge0 * diffY.y - edge1 * diffY.x) * r;
		//	//vertexVec[index0].tangent += tangent;
		//	//vertexVec[index1].tangent += tangent;
		//	//vertexVec[index2].tangent += tangent;
		//}
	
		//Fix the tangents per vertex now because we accumulated
		for (auto& v : vertexVec)
		{
		//	v.tangent = Vector3::Reject(v.tangent, v.normal).Normalized();
	
			if (flipAxisAndWinding)
			{
				v.Position.z *= -1.f;
				v.Normal.z *= -1.f;
				v.Color = glm::vec3{ 1.f, 1.f, 1.f };
				//v.tangent.z *= -1.f;
			}
	
		}
		
		return true;
	}

}
#endif