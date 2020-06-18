#pragma once

#include "Core/Core.h"
#include "Core/Debug/TimeStep.h"
#include "Core/Events/Events.h"

class Layers
{
public:
	Layers(const std::string& name = "Layer");
	virtual ~Layers() = default;

	virtual void OnAttach() {}
	virtual void OnDetach() {}
	virtual void OnUpdate(Timestep ts) {}
	virtual void OnImGuiRender() {}
	virtual void OnEvent(Event& event) {}

	const std::string& GetName() const { return m_DebugName; }
protected:
	std::string m_DebugName;
};


