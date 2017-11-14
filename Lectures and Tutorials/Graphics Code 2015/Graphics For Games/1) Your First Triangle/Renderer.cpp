#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent){

	triangle = Mesh::GenerateTriangle(0.0f, 0.5f, 0.0f);
	//triangle2 = Mesh::GenerateTriangle(0.7f, -0.5f, 0.0f);
	//triangle3 = Mesh::GenerateTriangle(0.0f, 0.0f, 0.5f);
	//square = Mesh::GenerateSquare(0.0f, 0.0f, 0.0f);

	currentShader = new Shader(SHADERDIR"basicVertex.glsl", SHADERDIR"colourFragment.glsl");

	if(!currentShader->LinkProgram()) {
		return;
	}

	init = true;
}

Renderer::~Renderer(void)	{

	delete triangle;
	//delete triangle2;
	//delete triangle3;
	//delete square;
}

void Renderer::RenderScene()	{

	//Sets the colour to which the window will be cleared
	glClearColor(0.2f,0.2f,0.2f,1.0f);
	//Actualy clears the window to that colour
	glClear(GL_COLOR_BUFFER_BIT);

	//Uses the shader loaded to draw the following objects
	glUseProgram(currentShader->GetProgram());
	triangle->Draw();
	//triangle2->Draw();
	//triangle3->Draw();
	//square->Draw();
	glUseProgram(0);

	SwapBuffers();	
}

