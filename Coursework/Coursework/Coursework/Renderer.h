#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/HeightMap.h"
#include "../../nclgl/textmesh.h"

class Renderer : public OGLRenderer {
public:
	Renderer(Window & parent);
	virtual ~Renderer(void);
	virtual void RenderScene();
	virtual void UpdateScene(float msec);
	void	DrawText(const std::string &text, const Vector3 &position, const float size = 10.0f, const bool perspective = false);

protected:
	Mesh * heightMap;
	Camera * camera;
	Light * light;
	Font*	basicFont;

	float counter, seconds;
	int frames, fps;
};