#include "PSIMPCH.h" 
#include "Model.h"

#ifdef TINYOBJLOADER_IMPLEMENTATION
#include <tinyObjLoader/tiny_obj_loader.h>
#elif TINYOBJ_LOADER_OPT_IMPLEMENTATION
#include <experimental/tinyobj_loader_opt.h>
#endif
#include <glm/gtx/hash.hpp>

namespace std {
	template<> struct hash<Mesh::HashVertex> {
		size_t operator()(Mesh::HashVertex const& vertex) const {
			return (((((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
		}
	};
}

Ref<Mesh> Mesh::Create(const std::string& filepath, bool load_normals, bool load_texcoords, bool generate_tangent_space_vectors, bool unify)
{
	return CreateRef<Mesh>(filepath, load_normals, load_texcoords, generate_tangent_space_vectors, unify);
}

Mesh::Mesh(const std::string& filepath, bool load_normals, bool load_texcoords, bool generate_tangent_space_vectors, bool unify)
{
	PSIM_PROFILE_FUNCTION();

	Load(filepath.c_str(), load_normals, load_texcoords, generate_tangent_space_vectors, unify);

	// Extract name from filepath
	auto lastSlash = filepath.find_last_of("/\\");
	lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
	auto lastDot = filepath.rfind('.');
	auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
	m_Name = filepath.substr(lastSlash, count);
}

Mesh::~Mesh()
{
	PSIM_PROFILE_FUNCTION();
}

bool Mesh::Load(char const * filename, bool load_normals, bool load_texcoords, bool generate_tangent_space_vectors, bool unify)
{
	PSIM_PROFILE_FUNCTION();
	// Load model

#ifdef TINYOBJLOADER_IMPLEMENTATION
	tinyobj::attrib_t attribs;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	{
		PSIM_PROFILE_SCOPE("Model File Read");
		std::string warn, err;

		//load model
		if (!tinyobj::LoadObj(&attribs, &shapes, &materials, &warn, &err, filename)) {
			PSIM_CORE_ERROR(warn + err);
		}
	}
#elif TINYOBJ_LOADER_OPT_IMPLEMENTATION
	tinyobj_opt::attrib_t attribs;
	std::vector<tinyobj_opt::shape_t> shapes;
	std::vector<tinyobj_opt::material_t> materials;
	{
		PSIM_PROFILE_SCOPE("Model File Read");
		//load model
		size_t data_len = 0;
		const char* data = nullptr;

#ifdef _WIN32
		HANDLE file = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
		assert(file != INVALID_HANDLE_VALUE);

		HANDLE fileMapping = CreateFileMapping(file, NULL, PAGE_READONLY, 0, 0, NULL);
		assert(fileMapping != INVALID_HANDLE_VALUE);

		LPVOID fileMapView = MapViewOfFile(fileMapping, FILE_MAP_READ, 0, 0, 0);
		auto fileMapViewChar = (const char*)fileMapView;
		assert(fileMapView != NULL);

		LARGE_INTEGER fileSize;
		fileSize.QuadPart = 0;
		GetFileSizeEx(file, &fileSize);

		data_len = static_cast<size_t>(fileSize.QuadPart);
		data = fileMapViewChar;
#endif
		PSIM_ASSERT(data != nullptr, "failed to load file\n");

		tinyobj_opt::LoadOption option;
		option.req_num_threads = 4;
		option.verbose = true;
		bool ret = tinyobj_opt::parseObj(&attribs, &shapes, &materials, data, data_len, option);
	}
#endif

	// Normal vectors and texture coordinates are required to generate tangent and bitangent vectors
	if (!load_normals || !load_texcoords) {
		generate_tangent_space_vectors = false;
	}

	// Load model data and unify (normalize) its size and position
	float min_x = attribs.vertices[0];
	float max_x = attribs.vertices[0];
	float min_y = attribs.vertices[1];
	float max_y = attribs.vertices[1];
	float min_z = attribs.vertices[2];
	float max_z = attribs.vertices[2];

#if generate_tangent_space_vectors == true
	std::unordered_map<HashVertexWithTanSpaceVec, uint32_t> uniqueVertices = {};
#else
	std::unordered_map<HashVertex, uint32_t> uniqueVertices = {};
	Data = {};
#endif

	uint32_t face_offset = 0;
	for (auto & shape : shapes) {
		uint32_t part_offset = face_offset;

#ifdef TINYOBJLOADER_IMPLEMENTATION
		for (auto & index : shape.mesh.indices) {

#if generate_tangent_space_vectors == true
			HashVertexWithTanSpaceVec vertex = {};
#else
			HashVertex vertex = {};
#endif

			vertex.pos = {
					attribs.vertices[3 * index.vertex_index + 0],
					attribs.vertices[3 * index.vertex_index + 1],
					attribs.vertices[3 * index.vertex_index + 2]
			};

			if (load_normals) {
				if (attribs.normals.size() == 0) {
					std::cout << "Could not load normal vectors data in the '" << filename << "' file.";
					return false;
				}
				else {
					vertex.normal = {
						attribs.normals[3 * index.normal_index + 0],
						attribs.normals[3 * index.normal_index + 1],
						attribs.normals[3 * index.normal_index + 2]
					};
				}
			}
			else {
				vertex.normal = { 0.0f, 0.0f, 0.0f };
			}

			if (load_texcoords) {
				if (attribs.texcoords.size() == 0) {
					std::cout << "Could not load texture coordinates data in the '" << filename << "' file.";
					return false;
				}
				else {
					vertex.texCoord = {
							attribs.texcoords[2 * index.texcoord_index + 0],
							1.0f - attribs.texcoords[2 * index.texcoord_index + 1]
					};
				}
			}

			if (generate_tangent_space_vectors) {
				// Insert temporary tangent space vectors data
				GenerateTangentSpaceVectors();
			}

			if (unify) {
				if (attribs.vertices[3 * index.vertex_index + 0] < min_x) {
					min_x = attribs.vertices[3 * index.vertex_index + 0];
				}
				if (attribs.vertices[3 * index.vertex_index + 0] > max_x) {
					max_x = attribs.vertices[3 * index.vertex_index + 0];
				}
				if (attribs.vertices[3 * index.vertex_index + 1] < min_y) {
					min_y = attribs.vertices[3 * index.vertex_index + 1];
				}
				if (attribs.vertices[3 * index.vertex_index + 1] > max_y) {
					max_y = attribs.vertices[3 * index.vertex_index + 1];
				}
				if (attribs.vertices[3 * index.vertex_index + 2] < min_z) {
					min_z = attribs.vertices[3 * index.vertex_index + 2];
				}
				if (attribs.vertices[3 * index.vertex_index + 2] > max_z) {
					max_z = attribs.vertices[3 * index.vertex_index + 2];
				}
			}

			vertex.color = { 1.0f, 1.0f, 1.0f };

			if (uniqueVertices.count(vertex) == 0) {
				uniqueVertices[vertex] = static_cast<uint32_t>(Data.size());
				Data.push_back(vertex.pos[0]);
				Data.push_back(vertex.pos[1]);
				Data.push_back(vertex.pos[2]);

				if (load_normals) {
					Data.push_back(vertex.normal[0]);
					Data.push_back(vertex.normal[1]);
					Data.push_back(vertex.normal[2]);
				}

				if (load_texcoords) {
					Data.push_back(vertex.texCoord[0]);
					Data.push_back(vertex.texCoord[1]);
				}

				Data.push_back(vertex.color[0]);
				Data.push_back(vertex.color[1]);
				Data.push_back(vertex.color[2]);

#if generate_tangent_space_vectors == true
				Data.push_back(vertex.tanSpaceVecP1[0]);
				Data.push_back(vertex.tanSpaceVecP1[1]);
				Data.push_back(vertex.tanSpaceVecP1[2]);

				Data.push_back(vertex.tanSpaceVecP2[0]);
				Data.push_back(vertex.tanSpaceVecP2[1]);
				Data.push_back(vertex.tanSpaceVecP2[2]);
#endif
			}

			indices.push_back(uniqueVertices[vertex]-1);
		}
#elif TINYOBJ_LOADER_OPT_IMPLEMENTATION
		for (size_t v = 0; v < attribs.face_num_verts.size(); v++) {
			PSIM_ASSERT(attribs.face_num_verts[v] % 3 == 0, "Not all faces are triangles"); // assume all triangle face.
			for (size_t f = 0; f < attribs.face_num_verts[v]; f++) {
#if generate_tangent_space_vectors == true
				HashVertexWithTanSpaceVec vertex = {};
#else
				HashVertex vertex = {};
#endif

				tinyobj_opt::index_t index = attribs.indices[face_offset + f];

				vertex.pos = {
					attribs.vertices[3 * index.vertex_index + 0],
					attribs.vertices[3 * index.vertex_index + 1],
					attribs.vertices[3 * index.vertex_index + 2]
				};

				if (load_normals) {
					if (attribs.normals.size() == 0) {
						PSIM_ERROR("Could not load normal vectors data in the ' {0} ' file.", filename);
						return false;
					}
					else {
						vertex.normal = {
							attribs.normals[3 * index.normal_index + 0],
							attribs.normals[3 * index.normal_index + 1],
							attribs.normals[3 * index.normal_index + 2]
						};
					}
				}
				else {
					vertex.normal = { 0.0f, 0.0f, 0.0f };
				}

				if (load_texcoords) {
					if (attribs.texcoords.size() == 0) {
						PSIM_ERROR("Could not load texture coordinates in the ' {0} ' file.", filename);
						return false;
					}
					else {
						vertex.texCoord = {
							attribs.texcoords[2 * index.texcoord_index + 0],
							1.0f - attribs.texcoords[2 * index.texcoord_index + 1]
						};
					}
				}

#if generate_tangent_space_vectors == true
					// Insert temporary tangent space vectors data
					vertex.tanSpaceVecP1 = {
							0.0f,
							0.0f,
							0.0f
					};
					vertex.tanSpaceVecP2 = {
							0.0f,
							0.0f,
							0.0f
					};
#endif

				if (unify) {
					if (attribs.vertices[3 * index.vertex_index + 0] < min_x) {
						min_x = attribs.vertices[3 * index.vertex_index + 0];
					}
					if (attribs.vertices[3 * index.vertex_index + 0] > max_x) {
						max_x = attribs.vertices[3 * index.vertex_index + 0];
					}
					if (attribs.vertices[3 * index.vertex_index + 1] < min_y) {
						min_y = attribs.vertices[3 * index.vertex_index + 1];
					}
					if (attribs.vertices[3 * index.vertex_index + 1] > max_y) {
						max_y = attribs.vertices[3 * index.vertex_index + 1];
					}
					if (attribs.vertices[3 * index.vertex_index + 2] < min_z) {
						min_z = attribs.vertices[3 * index.vertex_index + 2];
					}
					if (attribs.vertices[3 * index.vertex_index + 2] > max_z) {
						max_z = attribs.vertices[3 * index.vertex_index + 2];
					}
				}

				vertex.color = { 1.0f, 1.0f, 1.0f };

				if (uniqueVertices.count(vertex) == 0) {
					uniqueVertices[vertex] = static_cast<uint32_t>(Data.size()/8);
					Data.push_back(vertex.pos[0]);
					Data.push_back(vertex.pos[1]);
					Data.push_back(vertex.pos[2]);

					if (load_normals) {
						Data.push_back(vertex.normal[0]);
						Data.push_back(vertex.normal[1]);
						Data.push_back(vertex.normal[2]);
					}

					Data.push_back(vertex.texCoord[0]);
					Data.push_back(vertex.texCoord[1]);

					Data.push_back(vertex.color[0]);
					Data.push_back(vertex.color[1]);
					Data.push_back(vertex.color[2]);

#if generate_tangent_space_vectors == true
					Data.push_back(vertex.tanSpaceVecP1[0]);
					Data.push_back(vertex.tanSpaceVecP1[1]);
					Data.push_back(vertex.tanSpaceVecP1[2]);

					Data.push_back(vertex.tanSpaceVecP2[0]);
					Data.push_back(vertex.tanSpaceVecP2[1]);
					Data.push_back(vertex.tanSpaceVecP2[2]);
#endif
				}

				indices.push_back(uniqueVertices[vertex]);
			}
			face_offset += attribs.face_num_verts[v];
		}
#endif

		uint32_t part_vertex_count = face_offset - part_offset;
		if (0 < part_vertex_count) {
			Parts.push_back({ part_offset, part_vertex_count });
		}
	}

	uint32_t stride = 3 + 3 + (load_normals ? 3 : 0) + (load_texcoords ? 2 : 0) + (generate_tangent_space_vectors ? 6 : 0);
		std::cout << stride * sizeof(float) << std::endl;

	if (generate_tangent_space_vectors) {
		GenerateTangentSpaceVectors();
	}

	if (unify) {
		float offset_x = 0.5f * (min_x + max_x);
		float offset_y = 0.5f * (min_y + max_y);
		float offset_z = 0.5f * (min_z + max_z);
		float scale_x = abs(min_x - offset_x) > abs(max_x - offset_x) ? abs(min_x - offset_x) : abs(max_x - offset_x);
		float scale_y = abs(min_y - offset_y) > abs(max_y - offset_y) ? abs(min_y - offset_y) : abs(max_y - offset_y);
		float scale_z = abs(min_z - offset_z) > abs(max_z - offset_z) ? abs(min_z - offset_z) : abs(max_z - offset_z);
		float scale = scale_x > scale_y ? scale_x : scale_y;
		scale = scale_z > scale ? 1.0f / scale_z : 1.0f / scale;

		for (size_t i = 0; i < Data.size() - 2; i += stride) {
			Data[i + 0] = scale * (Data[i + 0] - offset_x);
			Data[i + 1] = scale * (Data[i + 1] - offset_y);
			Data[i + 2] = scale * (Data[i + 2] - offset_z);
		}
	}

	return true;
}

// Based on:
// Lengyel, Eric. "Computing Tangent Space Basis Vectors for an Arbitrary Mesh". Terathon Software 3D Graphics Library, 2001.
// http://www.terathon.com/code/tangent.html

void CalculateTangentAndBitangent(float const   * normal_data,
	glm::vec3 const & face_tangent,
	glm::vec3 const & face_bitangent,
	float         * tangent_data,
	float         * bitangent_data) {
	// Gram-Schmidt orthogonalize
	glm::vec3 const normal = { normal_data[0], normal_data[1], normal_data[2] };
	glm::vec3 const tangent = glm::normalize(face_tangent - normal * glm::dot(normal, face_tangent));

	// Calculate handedness
	float handedness = (glm::dot(glm::cross(normal, tangent), face_bitangent) < 0.0f) ? -1.0f : 1.0f;

	glm::vec3 const bitangent = handedness * glm::cross(normal, tangent);

	tangent_data[0] = tangent[0];
	tangent_data[1] = tangent[1];
	tangent_data[2] = tangent[2];

	bitangent_data[0] = bitangent[0];
	bitangent_data[1] = bitangent[1];
	bitangent_data[2] = bitangent[2];
}

// Based on:
	// Lengyel, Eric. "Computing Tangent Space Basis Vectors for an Arbitrary Mesh". Terathon Software 3D Graphics Library, 2001.
	// http://www.terathon.com/code/tangent.html

void Mesh::GenerateTangentSpaceVectors() {
	size_t const normal_offset = 3;
	size_t const texcoord_offset = 6;
	size_t const tangent_offset = 8;
	size_t const bitangent_offset = 11;
	size_t const stride = bitangent_offset + 3;

	for (auto & part : Parts) {
		for (size_t i = 0; i < Data.size(); i += stride * 3) {
			size_t i1 = i;
			size_t i2 = i1 + stride;
			size_t i3 = i2 + stride;
			glm::vec3 const v1 = { Data[i1], Data[i1 + 1], Data[i1 + 2] };
			glm::vec3 const v2 = { Data[i2], Data[i2 + 1], Data[i2 + 2] };
			glm::vec3 const v3 = { Data[i3], Data[i3 + 1], Data[i3 + 2] };

			std::array<float, 2> const w1 = { Data[i1 + texcoord_offset], Data[i1 + texcoord_offset + 1] };
			std::array<float, 2> const w2 = { Data[i2 + texcoord_offset], Data[i2 + texcoord_offset + 1] };
			std::array<float, 2> const w3 = { Data[i3 + texcoord_offset], Data[i3 + texcoord_offset + 1] };

			float x1 = v2[0] - v1[0];
			float x2 = v3[0] - v1[0];
			float y1 = v2[1] - v1[1];
			float y2 = v3[1] - v1[1];
			float z1 = v2[2] - v1[2];
			float z2 = v3[2] - v1[2];

			float s1 = w2[0] - w1[0];
			float s2 = w3[0] - w1[0];
			float t1 = w2[1] - w1[1];
			float t2 = w3[1] - w1[1];

			float r = 1.0f / (s1 * t2 - s2 * t1);
			glm::vec3 face_tangent = { (t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r };
			glm::vec3 face_bitangent = { (s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r };

			CalculateTangentAndBitangent(&Data[i1 + normal_offset], face_tangent, face_bitangent, &Data[i1 + tangent_offset], &Data[i1 + bitangent_offset]);
			CalculateTangentAndBitangent(&Data[i2 + normal_offset], face_tangent, face_bitangent, &Data[i2 + tangent_offset], &Data[i2 + bitangent_offset]);
			CalculateTangentAndBitangent(&Data[i3 + normal_offset], face_tangent, face_bitangent, &Data[i3 + tangent_offset], &Data[i3 + bitangent_offset]);
		}
	}
}

void ModelLibrary::Add(const std::string& name, const Ref<Mesh>& mesh)
{
	PSIM_ASSERT(!Exists(name), "Shader already exists!");
	m_Meshes[name] = mesh;

}

void ModelLibrary::Add(const Ref<Mesh>& mesh)
{
	auto& name = mesh->GetName();
	Add(name, mesh);
}
Ref<Mesh> ModelLibrary::Load(const std::string& filepath, bool load_normals, bool load_texcoords, bool generate_tangent_space_vectors, bool unify)
{
	auto mesh = Mesh::Create(filepath, load_normals, load_texcoords, generate_tangent_space_vectors, unify);
	Add(mesh);
	return mesh;
}

Ref<Mesh> ModelLibrary::Load(const std::string& name, const std::string& filepath, bool load_normals, bool load_texcoords, bool generate_tangent_space_vectors, bool unify)
{
	auto mesh = Mesh::Create(filepath, load_normals, load_texcoords, generate_tangent_space_vectors, unify);
	Add(name, mesh);
	return mesh;
}

Ref<Mesh> ModelLibrary::Get(const std::string& name)
{
	PSIM_ASSERT(Exists(name), "Shader not found!");
	return m_Meshes[name];
}

bool ModelLibrary::Exists(const std::string& name) const
{
	return m_Meshes.find(name) != m_Meshes.end();
}