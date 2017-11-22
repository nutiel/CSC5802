#include "Pyramid.h"

Mesh * Pyramid::pyramid = NULL;

Pyramid::Pyramid(Vector3 p) : SceneNode(pyramid, Vector4(1, 1, 1, 1.0)) {
	this->SetModelScale(Vector3(300, 300, 300));
	this->SetTransform(Matrix4::Translation(Vector3(90,0,0)));
	this->SetTransform(Matrix4::Rotation(90.0, Vector3(-1,0,0)));

	this->SetBoundingRadius(250.0f);
}

void Pyramid::Update(float msec) {

	SceneNode::Update(msec);
}