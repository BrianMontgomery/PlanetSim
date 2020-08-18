#include "PSIMPCH.h"
#include "Texture.h"
/*
void TextureLibrary::Add(const std::string& name, const Ref<Texture>& texture)
{
	PSIM_ASSERT(!Exists(name), "Shader already exists!");
	m_Textures[name] = texture;
}

void TextureLibrary::Add(const Ref<Texture>& texture)
{
	auto& name = texture->GetName();
	Add(name, texture);
}
Ref<Texture> TextureLibrary::Load(const std::string& filepath, bool load_normals, bool load_texcoords, bool generate_tangent_space_vectors, bool unify)
{
	auto texture = Texture::Create(filepath, load_normals, load_texcoords, generate_tangent_space_vectors, unify);
	Add(texture);
	return texture;
}

Ref<Texture> TextureLibrary::Load(const std::string& name, const std::string& filepath, bool load_normals, bool load_texcoords, bool generate_tangent_space_vectors, bool unify)
{
	auto texture = Texture::Create(filepath, load_normals, load_texcoords, generate_tangent_space_vectors, unify);
	Add(name, texture);
	return texture;
}

Ref<Texture> TextureLibrary::Get(const std::string& name)
{
	PSIM_ASSERT(Exists(name), "Shader not found!");
	return m_Textures[name];
}

bool TextureLibrary::Exists(const std::string& name) const
{
	return m_Textures.find(name) != m_Textures.end();
}*/