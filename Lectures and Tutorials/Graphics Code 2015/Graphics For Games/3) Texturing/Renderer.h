#pragma once
#include "../../nclgl/OGLRenderer.h"
# include "../../nclgl/Camera.h"

class Renderer : public OGLRenderer {
public:
	Renderer(Window & parent);
	virtual ~Renderer(void);
	virtual void RenderScene();
	void UpdateTextureMatrix(float rotation);
	void ToggleRepeating();
	void ToggleFiltering();
	bool getFiltering() { return filtering; }
	bool getRepeating() { return repeating; }
	virtual void UpdateScene(float msec);
protected:
	Camera* camera;
	Mesh * triangle;
	float timer;
	bool filtering;
	bool repeating;

};