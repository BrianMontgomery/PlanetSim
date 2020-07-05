#pragma once

#include "PSIM/Graphics/RenderAPI/RendererAPI.h"

class VulkanRendererAPI : public RendererAPI
{
public:
	virtual void Init() override;
	void SetViewProjectionMatrix(glm::mat4 viewProjectionMatrix) override;
	void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override {};

	virtual void SetClearColor(const glm::vec4& color) override;
	virtual void Clear() override;

	//virtual void DrawIndexed(const Ref<VertexArray>& vertexArray) override;
};