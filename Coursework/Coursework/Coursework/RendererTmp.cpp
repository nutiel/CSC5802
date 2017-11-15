#include "RendererTmp.h"

Renderer::Renderer(Window & parent) : OGLRenderer(parent) {

	camera = new Camera(0.0f, -90.0f, 0.0f, Vector3(RAW_WIDTH * HEIGHTMAP_X / 2.0f, 500, RAW_HEIGHT * HEIGHTMAP_Z));
	quad = Mesh::GenerateQuad();

	floorShader = new Shader(SHADERDIR"BumpVertex.glsl", SHADERDIR"BumpFragment.glsl");
	textShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"TexturedFragment.glsl");
	skyboxShader = new Shader(SHADERDIR"SkyboxVertex.glsl", SHADERDIR"SkyboxFragment.glsl");
	
	heightMap = new HeightMap(TEXTUREDIR"terrain.raw");
	heightMap->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	heightMap->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR"Barren RedsDOT3.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	if (!textShader->LinkProgram() || !floorShader->LinkProgram() || !heightMap->GetTexture() || !heightMap->GetBumpMap() || !skyboxShader->LinkProgram()) {
		return;
	}

	basicFont = new Font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT), 16, 16);
	cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR"rusted_west.jpg", TEXTUREDIR"rusted_east.jpg", TEXTUREDIR"rusted_up.jpg",
		TEXTUREDIR"rusted_down.jpg", TEXTUREDIR"rusted_south.jpg", TEXTUREDIR"rusted_north.jpg", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	/*cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR"nevada_rt.tga", TEXTUREDIR"nevada_lf.tga", TEXTUREDIR"nevada_up.tga",
		TEXTUREDIR"nevada_dn.tga", TEXTUREDIR"nevada_bk.tga", TEXTUREDIR"nevada_ft.tga", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);*/

	if (!cubeMap) {
		return;
	}

	SetTextureRepeating(heightMap->GetTexture(), true);
	SetTextureRepeating(heightMap->GetBumpMap(), true);

	light = new Light(Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f), 500.0f, (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f)), Vector4(1, 1, 1, 1), (RAW_WIDTH * HEIGHTMAP_X) / 2.0f);
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	counter = 0.0f;
	frames = 0;
	fps = 0;
	seconds2 = 0;

	init = true;
}

Renderer ::~Renderer(void) {
	delete camera;
	delete heightMap;
	delete light;
	delete basicFont;
	delete skyboxShader;
	delete quad;
}

void Renderer::UpdateScene(float msec) {
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
	seconds = msec;
	seconds2 += msec;

	light->SetPosition(Vector3(1500.0f*cos(seconds2 / 2000.0) + (RAW_HEIGHT * HEIGHTMAP_X / 2.0f), 500.0f, 1500.0f*sin(seconds2 / 2000.0) + (RAW_HEIGHT * HEIGHTMAP_X / 2.0f)));
}

void Renderer::RenderScene() {

	counter += seconds;
	frames++;

	if (counter >= 1000.0f) {
		counter = 0.0f;
		fps = frames;
		frames = 0;
	}

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	SetCurrentShader(textShader);
	glUniform1i(glGetUniformLocation(textShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(textShader->GetProgram(), "bumpTex"), 1);

	glUniform3fv(glGetUniformLocation(textShader->GetProgram(), "cameraPos"), 1, (float *)& camera->GetPosition());

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	DrawText("FPS: " + std::to_string(fps), Vector3(0, 0, 0), 16.0f);
	glUseProgram(0);

	glDepthMask(GL_FALSE);
	SetCurrentShader(skyboxShader);

	UpdateShaderMatrices();
	quad->Draw();

	glUseProgram(0);
	glDepthMask(GL_TRUE);

	SetCurrentShader(floorShader);
	glUniform1i(glGetUniformLocation(floorShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(floorShader->GetProgram(), "bumpTex"), 1);

	glUniform3fv(glGetUniformLocation(floorShader->GetProgram(), "cameraPos"), 1, (float *)& camera->GetPosition());


	glBlendFunc(GL_SRC_ALPHA, GL_NONE);
	glEnable(GL_DEPTH_TEST);

	UpdateShaderMatrices();
	SetShaderLight(*light);

	heightMap->Draw();
	
	glUseProgram(0);
	SwapBuffers();
}

void Renderer::DrawText(const std::string &text, const Vector3 &position, const float size, const bool perspective) {
	//Create a new temporary TextMesh, using our line of text and our font
	TextMesh* mesh = new TextMesh(text, *basicFont);

	Matrix4 temp_modelMatrix = modelMatrix;
	Matrix4 temp_viewMatrix = viewMatrix;
	Matrix4 temp_projMatrix = projMatrix;
	//This just does simple matrix setup to render in either perspective or
	//orthographic mode, there's nothing here that's particularly tricky.
	if (perspective) {
		modelMatrix = Matrix4::Translation(position) * Matrix4::Scale(Vector3(size, size, 1));
		viewMatrix = camera->BuildViewMatrix();
		projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	}
	else {
		//In ortho mode, we subtract the y from the height, so that a height of 0
		//is at the top left of the screen, which is more intuitive
		//(for me anyway...)
		modelMatrix = Matrix4::Translation(Vector3(position.x, height - position.y, position.z)) * Matrix4::Scale(Vector3(size, size, 1));
		viewMatrix.ToIdentity();
		projMatrix = Matrix4::Orthographic(-1.0f, 1.0f, (float)width, 0.0f, (float)height, 0.0f);
	}
	//Either way, we update the matrices, and draw the mesh
	UpdateShaderMatrices();
	mesh->Draw();

	modelMatrix = temp_modelMatrix;
	viewMatrix = temp_viewMatrix;
	projMatrix = temp_projMatrix;
	
	UpdateShaderMatrices();
	delete mesh; //Once it's drawn, we don't need it anymore!
}