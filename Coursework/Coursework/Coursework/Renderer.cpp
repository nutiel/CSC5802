#include "Renderer.h"

Renderer::Renderer(Window & parent) : OGLRenderer(parent) {

	CubeMap::CreateCubeMap();
	camera = new Camera(0.0f, -90.0f, 0.0f, Vector3(RAW_WIDTH * HEIGHTMAP_X / 2.0f, 500, RAW_HEIGHT * HEIGHTMAP_Z));

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

	camera->SetPosition(Vector3(0, 30, 175));

	root = new SceneNode();
	root->AddChild(new CubeMap());

	glEnable(GL_DEPTH_TEST);
	init = true;

}

Renderer ::~Renderer(void) {
	delete root;
	CubeMap::DeleteCube(); // Also important !
}