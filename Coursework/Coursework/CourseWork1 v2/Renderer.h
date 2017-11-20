#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/HeightMap.h"
#include "../../nclgl/textmesh.h"
#include "../../nclgl/SceneNode.h"
#include "../../nclgl/Frustum.h"
#include "Cube.h"
#include "Sphere.h"
#include <algorithm>

class Renderer : public OGLRenderer {
public:
	Renderer(Window & parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	void RenderScene2();
	virtual void UpdateScene(float msec, bool switchScene);

	void resetFade();
	int getFade();

	void DrawText(const std::string &text, const Vector3 &position, const float size = 10.0f, const bool perspective = false);

protected:
	Mesh * heightMap;
	Mesh * quad;
	Camera * camera;
	Light * light;
	Font*	basicFont;
	Shader *textShader, *floorShader, *skyboxShader, *eyeShader, *regularShader;
	GLuint cubeMap, cubeMap2;


	void BuildNodeLists(SceneNode * from);
	void SortNodeLists();
	void ClearNodeLists();
	void DrawNodes();
	void DrawNode(SceneNode * n);
	SceneNode * root;
	Frustum frameFrustum;
	vector < SceneNode * > transparentNodeList;
	vector < SceneNode * > nodeList;

	float counter, seconds, seconds2, fade;
	int frames, fps;

	bool switchScene;
	void showFPS();
	void drawSkyBox();
};