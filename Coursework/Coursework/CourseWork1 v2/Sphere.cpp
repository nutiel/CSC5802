#include "Sphere.h"

Mesh * Sphere::sphere = NULL;

Sphere::Sphere(Vector3 p) : SceneNode(sphere, Vector4(1, 1, 0, 0.5)) {
	this->SetModelScale(Vector3(250, 250, 250));
	this->SetTransform(Matrix4::Translation(p));

	this->SetBoundingRadius(250.0f);
}

void Sphere::Update(float msec) {
	
	SceneNode::Update(msec);
}