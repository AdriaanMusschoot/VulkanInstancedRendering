#ifndef VK_FILEREADER_OBJ_H
#define VK_FILEREADER_OBJ_H
#include "Engine/Configuration.h"
#include "RenderStructs.h"
namespace vkUtil
{
	template<typename VertexStruct>
	bool ParseOBJ(const std::string& filename, std::vector<VertexStruct>& vertexVec, std::vector<uint32_t>& indexVec, bool flipAxisAndWinding = true)
	{
		std::ifstream file{ filename };
		if (!file)
		{
			return false;
		}

		std::vector<glm::vec3> positionvec{};
		std::vector<glm::vec3> normalvec{};
		std::vector<glm::vec2> uvvec{};

		vertexVec.clear();
		indexVec.clear();

		std::string scommand;
		// start a while iteration ending when the end of file is reached (ios::eof)
		while (!file.eof())
		{
			//read the first word of the string, use the >> operator (istream::operator>>) 
			file >> scommand;
			//use conditional statements to process the different commands	
			if (scommand == "#")
			{
				// ignore comment
			}
			else if (scommand == "v")
			{
				//vertex
				float x, y, z;
				file >> x >> y >> z;

				positionvec.emplace_back(glm::vec3{ x, y, z });
			}
			else if (scommand == "vt")
			{
				// vertex texcoord
				float u, v;
				file >> u >> v;
				uvvec.emplace_back(u, 1 - v);
			}
			else if (scommand == "vn")
			{
				// vertex normal
				float x, y, z;
				file >> x >> y >> z;

				normalvec.emplace_back(glm::vec3{ x, y, z });
			}
			else if (scommand == "f")
			{
				//if a face is read:
				//construct the 3 vertices, add them to the vertex array
				//add three indices to the index array
				//add the material index as attibute to the attribute array
				//
				// faces or triangles
				VertexStruct vertex{};
				size_t idxposition, idxtexcoord, idxnormal;

				uint32_t tempindexvec[3];
				for (size_t idxface = 0; idxface < 3; idxface++)
				{
					// obj format uses 1-based arrays
					file >> idxposition;
					vertex.Position = positionvec[idxposition - 1];

					if ('/' == file.peek())//is next in buffer ==  '/' ?
					{
						file.ignore();//read and ignore one element ('/')

						if ('/' != file.peek())
						{
							// optional texture coordinate
							file >> idxtexcoord;
							//vertex.uv = uvvec[itexcoord - 1];
						}

						if ('/' == file.peek())
						{
							file.ignore();

							// optional vertex normal
							file >> idxnormal;
							vertex.Normal = normalvec[idxnormal - 1];
						}
					}

					vertexVec.emplace_back(vertex);
					tempindexvec[idxface] = uint32_t(vertexVec.size()) - 1;
				}

				indexVec.emplace_back(tempindexvec[0]);
				if (flipAxisAndWinding)
				{
					indexVec.emplace_back(tempindexvec[2]);
					indexVec.emplace_back(tempindexvec[1]);
				}
				else
				{
					indexVec.emplace_back(tempindexvec[1]);
					indexVec.emplace_back(tempindexvec[2]);
				}
			}
			//read till end of line and ignore all remaining chars
			file.ignore(1000, '\n');
		}

		//cheap tangent calculations
		//for (uint32_t i = 0; i < indexvec.size(); i += 3)
		//{
		//	uint32_t index0 = indexvec[i];
		//	uint32_t index1 = indexvec[size_t(i) + 1];
		//	uint32_t index2 = indexvec[size_t(i) + 2];
		//
		//	const glm::vec3& p0 = vertexvec[index0].position;
		//	const glm::vec3& p1 = vertexvec[index1].position;
		//	const glm::vec3& p2 = vertexvec[index2].position;
		//	//const vector2& uv0 = vertexvec[index0].uv;
		//	//const vector2& uv1 = vertexvec[index1].uv;
		//	//const vector2& uv2 = vertexvec[index2].uv;
		//
		//	//const glm::vec3 edge0 = p1 - p0;
		//	//const glm::vec3 edge1 = p2 - p0;
		//	//const vector2 diffx = vector2(uv1.x - uv0.x, uv2.x - uv0.x);
		//	//const vector2 diffy = vector2(uv1.y - uv0.y, uv2.y - uv0.y);
		//	//float r = 1.f / vector2::cross(diffx, diffy);
		//
		//	//vector3 tangent = (edge0 * diffy.y - edge1 * diffy.x) * r;
		//	//vertexvec[index0].tangent += tangent;
		//	//vertexvec[index1].tangent += tangent;
		//	//vertexvec[index2].tangent += tangent;
		//}

		//fix the tangents per vertex now because we accumulated
		for (auto& v : vertexVec)
		{
			//	v.tangent = vector3::reject(v.tangent, v.normal).normalized();

			if (flipAxisAndWinding)
			{
				v.Position.z *= 1.f;
				v.Normal.z *= -1.f;
				v.Color = glm::vec3{ 1.f, 1.f, 1.f };
				//v.tangent.z *= -1.f;
			}

		}
		return true;
	}
}
#endif