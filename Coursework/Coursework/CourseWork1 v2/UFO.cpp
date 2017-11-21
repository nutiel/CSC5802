#include "UFO.h"

Mesh * UFO::sphere = NULL;

UFO::UFO(Vector3 p) : SceneNode(sphere, Vector4(1, 1, 1, 1)) {

	top = new SceneNode(sphere, Vector4(1,1,0,1));
	top->SetModelScale(Vector3(1, 1, 1));
	top->SetTransform(Matrix4::Translation(Vector3(100, 100, 100)));
	top->setShader(this->getShader());
	AddChild(top);

	body = new SceneNode(sphere, Vector4(0.5,0.5,0.5,1.0));
	body->SetModelScale(Vector3(1, 0.3, 1));
	body->SetTransform(Matrix4::Translation(Vector3(100, 100, 100)));
	body->setShader(this->getShader());
	AddChild(body);

	this->SetBoundingRadius(25.0f);

	
}

void UFO::Update(float msec) {

	SceneNode::Update(msec);
}