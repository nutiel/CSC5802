#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/HeightMap.h"
#include "../../nclgl/textmesh.h"
#include "../../nclgl/SceneNode.h"
#include "../../nclgl/Frustum.h"
#include"ParticleEmitter.h"
#include "Cube.h"
#include "Sphere.h"
#include "Pyramid.h"
#include "UFO.h"
#include <algorithm>

class Renderer : public OGLRenderer {
public:
	Renderer(Window & parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec, int switchScene, int scene);

	void resetFade();
	int getFade();

	void DrawText(const std::string &text, const Vector3 &position, const float size = 10.0f, const bool perspective = false);

protected:
	Mesh * heightMap;
	Mesh * quad;
	Camera * camera;
	Light * light;
	Font*	basicFont;
	Shader *textShader, *floorShader, *skyboxShader, *eyeShader, *regularShader, *sceneGraph, *particleShader;
	GLuint cubeMap, cubeMap2;
	GLuint desertTexture, barrenTexture, brickTexture;
	GLuint barrenBump, brickBump;

	void BuildNodeLists(SceneNode * from);
	void SortNodeLists();
	void ClearNodeLists();
	void DrawNodes();
	void DrawNode(SceneNode * n);
	void DrawParticles();
	void SetShaderParticleSize(float f);	//And a new setter

	ParticleEmitter*	emitter;	//A single particle emitter
	SceneNode * root;
	Frustum frameFrustum;
	vector < SceneNode * > transparentNodeList;
	vector < SceneNode * > nodeList;

	float counter, seconds, seconds2, fade;
	int frames, fps, currentScene, switchScene;

	bool perform_once;

	void showFPS();
	void drawSkyBox();
	void drawTerrain();
};