#pragma once

#include "PlanetSimEngine.h"

class PSIMAppLayer : public Layers
{
public:
	PSIMAppLayer();
	virtual ~PSIMAppLayer() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	void OnUpdate(Timestep ts) override;
	virtual void OnImGuiRender() override;
	void OnEvent(Event& e) override;
private:
	//ShaderLibrary m_ShaderLibrary;
	//Ref<Shader> m_Shader;
	Ref<VertexArray> m_VertexArray;
	ModelLibrary m_ModelLibrary;

	//Ref<Shader> m_FlatColorShader;
	Ref<VertexArray> m_SquareVA;

	//Ref<Texture2D> m_Texture, m_ChernoLogoTexture;

	CameraController m_CameraController;
	glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.8f };
};