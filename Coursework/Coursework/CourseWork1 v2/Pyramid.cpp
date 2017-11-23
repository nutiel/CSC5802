#include "Pyramid.h"

Mesh * Pyramid::pyramid = NULL;

Pyramid::Pyramid(Vector3 p) : SceneNode(pyramid, Vector4(1, 1, 1, 1.0)) {
	this->SetModelScale(Vector3(100, 100, 100));

	this->SetBoundingRadius(250.0f);
}

void Pyramid::Update(float msec) {

	SceneNode::Update(msec);
}