#pragma once

#include "../../nclgl/scenenode.h"
#include "../../nclgl/OBJMesh.h"

class Sphere : public SceneNode {
public:
	Sphere(Vector3 p);
	~Sphere() {};
	virtual void Update(float msec);

	static void CreateSphere() {
		OBJMesh* m = new OBJMesh();
		m->LoadOBJMesh(MESHDIR"ico.obj");
		sphere = m;
	}
	static void DeleteSphere() {
		delete sphere;
	}

protected:
	static Mesh * sphere;
};