#include "Renderer.h"

Renderer::Renderer(Window & parent) : OGLRenderer(parent) {
	camera = new Camera();
	//triangle = Mesh::GenerateTriangle(0.0f, 0.5f, 0.0f);
	triangle = Mesh::GenerateSquare(0.0f, 0.0f, 0.0f);

	triangle -> SetTexture(SOIL_load_OGL_texture("../../Textures/rusted_south.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0));

	timer = 0.0f;

	if (!triangle -> GetTexture()) {
		return;
	}
	currentShader = new Shader("../../Shaders/TexturedVertex.glsl", "../../Shaders/TexturedFragment.glsl");
	if (!currentShader -> LinkProgram()) {
		return;
	}
	init = true;
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, 1, -1);
	filtering = true;
	repeating = false;

}

Renderer ::~Renderer(void) {
	delete triangle;
	delete camera;
}

void Renderer::RenderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(currentShader->GetProgram());
	UpdateShaderMatrices();

	timer += 0.03;
	////////////////////////////////////////////////////////////////////////////
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "time"), timer);
	////////////////////////////////////////////////////////////////////////////

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	triangle->Draw();
	glUseProgram(0);

	SwapBuffers();
}

void Renderer::UpdateTextureMatrix(float value) {
	Matrix4 pushPos = Matrix4::Translation(Vector3(0.5f, 0.5f, 0));
	Matrix4 popPos = Matrix4::Translation(Vector3(-0.5f, -0.5f, 0));
	Matrix4 rotation = Matrix4::Rotation(value, Vector3(0, 0, 1));
	textureMatrix = pushPos * rotation * popPos;
}

void Renderer::ToggleRepeating() {
	repeating = !repeating;
	glBindTexture(GL_TEXTURE_2D, triangle->GetTexture());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, //x axis
		repeating ? GL_REPEAT : GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, //y axis
		repeating ? GL_REPEAT : GL_CLAMP);
	glBindTexture(GL_TEXTURE_2D, 0);

}

void Renderer::ToggleFiltering() {
	filtering = !filtering;
	glBindTexture(GL_TEXTURE_2D, triangle -> GetTexture());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		filtering ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
		filtering ? GL_LINEAR : GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::UpdateScene(float msec) {
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
}