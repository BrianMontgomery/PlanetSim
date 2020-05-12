#pragma once

class GraphicsContext
{
public:
	virtual void Init() = 0;
	virtual void drawFrame() = 0;

	virtual void SwapBuffers() = 0;

	static Scope<GraphicsContext> Create(void* window);
};