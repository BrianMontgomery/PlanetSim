#pragma once 

#include <vector>
#include <glm/glm.hpp>

struct Mesh {
	static Ref<Mesh> Create(const std::string& filepath, bool load_normals, bool load_texcoords, bool generate_tangent_space_vectors, bool unify);

	Mesh(const std::string& filepath, bool load_normals, bool load_texcoords, bool generate_tangent_space_vectors, bool unify);
	Mesh::~Mesh();

	const std::string& GetName() const { return m_Name; }

	//------------------------------------------------------

	std::vector<float>  Data;
	std::vector<uint32_t> indices;

	struct Part {
		uint32_t  VertexOffset;
		uint32_t  VertexCount;
	};

	struct HashVertex {
		glm::vec3 pos;
		glm::vec2 texCoord;
		glm::vec3 color;
		glm::vec3 normal;

		bool operator==(const HashVertex& other) const {
			return pos == other.pos && color == other.color && texCoord == other.texCoord && normal == other.normal;
		}
	};

	struct HashVertexWithTanSpaceVec {
		glm::vec3 pos;
		glm::vec2 texCoord;
		glm::vec3 color;
		glm::vec3 normal;
		glm::vec3 tanSpaceVecP1;
		glm::vec3 tanSpaceVecP2;

		bool operator==(const HashVertexWithTanSpaceVec& other) const {
			return pos == other.pos && color == other.color && texCoord == other.texCoord && normal == other.normal && tanSpaceVecP1 == other.tanSpaceVecP1 && tanSpaceVecP2 == other.tanSpaceVecP2;
		}
	};

	std::vector<Part> Parts;

	//------------------------------------------------------
private:
	bool Load(char const * filename, bool load_normals, bool load_texcoords, bool generate_tangent_space_vectors, bool unify);
	void GenerateTangentSpaceVectors();

	//------------------------------------------------------

	std::string m_Name;
};

class ModelLibrary {
public:
	void Add(const Ref<Mesh>& mesh);
	void Add(const std::string& name, const Ref<Mesh>& mesh);

	Ref<Mesh> Load(const std::string& filepath, bool load_normals, bool load_texcoords, bool generate_tangent_space_vectors, bool unify);
	Ref<Mesh> Load(const std::string& name, const std::string& filepath, bool load_normals, bool load_texcoords, bool generate_tangent_space_vectors, bool unify);

	Ref<Mesh> Get(const std::string& name);

	bool Exists(const std::string& name) const;

private:
	std::unordered_map<std::string, Ref<Mesh>> m_Meshes;
};