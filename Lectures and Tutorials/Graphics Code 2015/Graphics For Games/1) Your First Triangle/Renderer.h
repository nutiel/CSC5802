#pragma once

#include "../../nclgl/OGLRenderer.h"

class Renderer : public OGLRenderer	{
public:
	Renderer(Window &parent);
	virtual ~Renderer(void);
	virtual void RenderScene();

protected:
	Mesh* triangle;
	Mesh* triangle2;
	Mesh* triangle3;
	Mesh* square;
};
