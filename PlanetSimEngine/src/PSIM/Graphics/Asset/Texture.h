#pragma once
/*
#include <string>

#include "PSIM/Core/Core.h"

class Texture
{
public:
	static Ref<Mesh> Create(const std::string& filepath, bool load_normals, bool load_texcoords, bool generate_tangent_space_vectors, bool unify);

	Mesh(const std::string& filepath, bool load_normals, bool load_texcoords, bool generate_tangent_space_vectors, bool unify);
	Mesh::~Mesh();

	const std::string& GetName() const { return m_Name; }
	virtual ~Texture() = default;

	virtual uint32_t GetWidth() const = 0;
	virtual uint32_t GetHeight() const = 0;

	virtual void SetData(void* data, uint32_t size) = 0;

	virtual void Bind(uint32_t slot = 0) const = 0;

	virtual bool operator==(const Texture& other) const = 0;
};

class TextureLibrary {
public:
	void Add(const Ref<Texture>& mesh);
	void Add(const std::string& name, const Ref<Texture>& mesh);

	Ref<Texture> Load(const std::string& filepath, bool load_normals, bool load_texcoords, bool generate_tangent_space_vectors, bool unify);
	Ref<Texture> Load(const std::string& name, const std::string& filepath, bool load_normals, bool load_texcoords, bool generate_tangent_space_vectors, bool unify);

	Ref<Texture> Get(const std::string& name);

	bool Exists(const std::string& name) const;

private:
	std::unordered_map<std::string, Ref<Texture>> m_Textures;
};*/