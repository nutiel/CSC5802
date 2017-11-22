#pragma once

#include "../../nclgl/scenenode.h"
#include "../../nclgl/OBJMesh.h"

class UFO : public SceneNode {
public:
	UFO(Vector3 p);
	~UFO() {};
	virtual void Update(float msec);

	static void CreateSphere() {
		OBJMesh* m = new OBJMesh();
		m->LoadOBJMesh(MESHDIR"ico.obj");
		sphere = m;
	}
	static void DeleteSphere() {
		delete sphere;
	}

	void createChildren();

protected:
	static Mesh * sphere;
	SceneNode *top, *body;
	
};