#include "Cube.h"

Mesh * Cube::cube = NULL;

Cube::Cube(Vector3 p) : SceneNode(cube, Vector4(1, 1, 1, 0.0)) {
	this->SetModelScale(Vector3(150, 150, 150));
	this->SetTransform(Matrix4::Translation(p));

	this->SetBoundingRadius(50.0f);
}

void Cube::Update(float msec) {

	SceneNode::Update(msec);
}