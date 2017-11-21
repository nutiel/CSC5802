#pragma once

#include "../../nclgl/scenenode.h"
#include "../../nclgl/OBJMesh.h"

class Cube : public SceneNode {
public:
	Cube(Vector3 p);
	~Cube() {};
	virtual void Update(float msec);

	static void CreateCube() {
		OBJMesh* m = new OBJMesh();
		m->LoadOBJMesh(MESHDIR"centeredcube.obj");
		cube = m;
	}
	static void DeleteCube() { 
		delete cube; 
	}

protected:
	static Mesh * cube;
};