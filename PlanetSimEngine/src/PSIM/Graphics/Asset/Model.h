#pragma once 

#include <vector>

struct Mesh {
	static Ref<Mesh> Create(const std::string& filepath, bool load_normals, bool load_texcoords, bool generate_tangent_space_vectors, bool unify, uint32_t vertex_stride = 0);

	Mesh(const std::string& filepath, bool load_normals, bool load_texcoords, bool generate_tangent_space_vectors, bool unify, uint32_t vertex_stride);
	Mesh::~Mesh();

	const std::string& GetName() const { return m_Name; }

	//------------------------------------------------------

	std::vector<float>  Data;

	struct Part {
		uint32_t  VertexOffset;
		uint32_t  VertexCount;
	};

	std::vector<Part> Parts;

	//------------------------------------------------------
private:
	bool Load(char const * filename, bool load_normals, bool load_texcoords, bool generate_tangent_space_vectors, bool unify, uint32_t vertex_stride = 0);
	void GenerateTangentSpaceVectors();

	//------------------------------------------------------
	
	struct HashVertex {
		glm::vec3 pos;
		glm::vec2 texCoord;
		glm::vec3 color;
		glm::vec3 normal;
	};

	struct HashVertexWithTanSpaceVec {
		glm::vec3 pos;
		glm::vec2 texCoord;
		glm::vec3 color;
		glm::vec3 normal;
		glm::vec3 tanSpaceVecP1;
		glm::vec3 tanSpaceVecP2;
	};

	std::string m_Name;
};

class ModelLibrary {
public:
	void Add(const Ref<Mesh>& mesh);
	void Add(const std::string& name, const Ref<Mesh>& mesh);

	Ref<Mesh> Load(const std::string& filepath, bool load_normals, bool load_texcoords, bool generate_tangent_space_vectors, bool unify, uint32_t vertex_stride);
	Ref<Mesh> Load(const std::string& name, const std::string& filepath, bool load_normals, bool load_texcoords, bool generate_tangent_space_vectors, bool unify, uint32_t vertex_stride);

	Ref<Mesh> Get(const std::string& name);

	bool Exists(const std::string& name) const;

private:
	std::unordered_map<std::string, Ref<Mesh>> m_Meshes;
};