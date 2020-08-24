#include "PSIMPCH.h"
#include "Texture.h"

#include "PSIM/Graphics/Renderer.h"
#include "Platform/Vk/Components/VulkanTexture2D.h"

Ref<Texture2D> Texture2D::Create(const std::string& name, uint32_t width, uint32_t height)
{
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::None:    PSIM_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	case RendererAPI::API::OpenGL:  PSIM_ASSERT(false, "RendererAPI::OpenGL is currently not supported!"); return nullptr;
	case RendererAPI::API::Vulkan:  return CreateRef<VulkanTexture2D>(name, width, height);
	}

	PSIM_ASSERT(false, "Unknown RendererAPI!");
	return nullptr;
}

Ref<Texture2D> Texture2D::Create(const std::string& path)
{
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::None:    PSIM_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	case RendererAPI::API::OpenGL:  PSIM_ASSERT(false, "RendererAPI::OpenGL is currently not supported!"); return nullptr;
	case RendererAPI::API::Vulkan:  return CreateRef<VulkanTexture2D>(path);
	}

	PSIM_ASSERT(false, "Unknown RendererAPI!");
	return nullptr;
}

void TextureLibrary::Add(const std::string& name, const Ref<Texture2D>& texture)
{
	PSIM_ASSERT(!Exists(name), "Shader already exists!");
	m_Textures[name] = texture;
}

void TextureLibrary::Add(const Ref<Texture2D>& texture)
{
	auto& name = texture->GetName();
	Add(name, texture);
}

Ref<Texture2D> TextureLibrary::Load(const std::string& name, uint32_t width, uint32_t height)
{
	auto texture = Texture2D::Create(name, width, height);
	Add(name, texture);
	return texture;
}

Ref<Texture2D> TextureLibrary::Load(const std::string& path)
{
	auto texture = Texture2D::Create(path);
	Add(texture);
	return texture;
}

Ref<Texture2D> TextureLibrary::Get(const std::string& name)
{
	PSIM_ASSERT(Exists(name), "Shader not found!");
	return m_Textures[name];
}

bool TextureLibrary::Exists(const std::string& name) const
{
	return m_Textures.find(name) != m_Textures.end();
}