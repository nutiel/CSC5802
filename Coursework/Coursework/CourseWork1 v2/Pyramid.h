#pragma once

#include "../../nclgl/scenenode.h"
#include "../../nclgl/OBJMesh.h"

class Pyramid : public SceneNode {
public:
	Pyramid(Vector3 p);
	~Pyramid() {};
	virtual void Update(float msec);

	static void CreatePyramid() {
		OBJMesh* m = new OBJMesh();
		m->LoadOBJMesh(MESHDIR"pyramid.obj");
		pyramid = m;
	}
	static void DeletePyramid() {
		delete pyramid;
	}

protected:
	static Mesh * pyramid;
};