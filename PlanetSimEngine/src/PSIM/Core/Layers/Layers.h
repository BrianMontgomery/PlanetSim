#pragma once

#include "PSIM/Core/Core.h"
#include "PSIM/Core/Debug/TimeStep.h"
#include "PSIM/Core/Events/Events.h"

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


