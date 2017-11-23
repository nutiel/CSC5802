#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/HeightMap.h"
#include "../../nclgl/textmesh.h"
#include "../../nclgl/SceneNode.h"
#include "../../nclgl/Frustum.h"
#include "../../nclgl/MD5Mesh.h"
#include "../../nclgl/MD5Node.h"
#include"ParticleEmitter.h"
#include "Cube.h"
#include "Sphere.h"
#include "Pyramid.h"
#include <algorithm>

# define SHADOWSIZE 2048

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

	HeightMap * heightMap;
	Mesh * quad;
	Camera * camera;
	Light * light;
	Font*	basicFont;
	Shader *textShader, *floorShader, *skyboxShader, *eyeShader, *regularShader, *sceneGraph, *particleShader, *lightShader, *reflectShader, *sceneShader, *shadowShader;
	GLuint cubeMap, cubeMap2, cubeMap3;
	GLuint desertTexture, barrenTexture, brickTexture, bushTexture;
	GLuint barrenBump, brickBump;
	GLuint shadowTex;

	GLuint shadowFBO;
	
	MD5FileData * hellData;
	MD5Node * hellNode;
	Mesh * floor;

	void BuildNodeLists(SceneNode * from);
	void SortNodeLists();
	void ClearNodeLists();
	void DrawNodes();
	void DrawNode(SceneNode * n);
	void DrawParticles();
	void SetShaderParticleSize(float f);	//And a new setter
	void DrawWater();

	void DrawMesh(); // New !
	void DrawFloor(); // New !
	void DrawShadowScene(); // New !
	void DrawCombinedScene(); // New !

	ParticleEmitter*	emitter;	//A single particle emitter
	SceneNode * root1, *root2, *root3;
	Frustum frameFrustum;
	vector < SceneNode * > transparentNodeList;
	vector < SceneNode * > nodeList;

	float counter, seconds, seconds2, fade, waterRotate, grow;
	int frames, fps, currentScene, switchScene;

	bool perform_once;

	void showFPS();
	void drawSkyBox();
	void drawTerrain();
};