#pragma once

#include "Core/Core.h"
#include "Core/Layers/Layers.h"

#include <vector>

class LayerStack
{
public:
	LayerStack() = default;
	~LayerStack();

	void PushLayer(Layers* layer);
	void PushOverlay(Layers* overlay);
	void PopLayer(Layers* layer);
	void PopOverlay(Layers* overlay);

	std::vector<Layers*>::iterator begin() { return m_Layers.begin(); }
	std::vector<Layers*>::iterator end() { return m_Layers.end(); }
	std::vector<Layers*>::reverse_iterator rbegin() { return m_Layers.rbegin(); }
	std::vector<Layers*>::reverse_iterator rend() { return m_Layers.rend(); }

	std::vector<Layers*>::const_iterator begin() const { return m_Layers.begin(); }
	std::vector<Layers*>::const_iterator end()	const { return m_Layers.end(); }
	std::vector<Layers*>::const_reverse_iterator rbegin() const { return m_Layers.rbegin(); }
	std::vector<Layers*>::const_reverse_iterator rend() const { return m_Layers.rend(); }
private:
	std::vector<Layers*> m_Layers;
	unsigned int m_LayerInsertIndex = 0;
};