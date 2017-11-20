#include "Sphere.h"

Mesh * Sphere::sphere = NULL;

Sphere::Sphere(Vector3 p) : SceneNode(sphere, Vector4(1, 1, 0, 0.5)) {
	this->SetModelScale(Vector3(150, 150, 150));
	this->SetTransform(Matrix4::Translation(p));

	this->SetBoundingRadius(50.0f);
}

void Sphere::Update(float msec) {
	
	SceneNode::Update(msec);
}